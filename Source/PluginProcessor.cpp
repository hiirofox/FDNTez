/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "dsp/fdn_reverb.h"

//==============================================================================
FDNTezAudioProcessor::FDNTezAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{/*
	pData = new PitchShiftData;
	InitPhasexPhaser(&pData->phsl);
	InitPhasexPhaser(&pData->phsr);
	*/
}

juce::AudioProcessorValueTreeState::ParameterLayout FDNTezAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;
	layout.add(std::make_unique<juce::AudioParameterFloat>("sizel", "SIZEL", 0, 1.0, 0.75));
	layout.add(std::make_unique<juce::AudioParameterFloat>("sizer", "SIZER", 0, 1.0, 0.75));
	layout.add(std::make_unique<juce::AudioParameterFloat>("sizmode", "mode", 0, 1.0, 0.75));

	layout.add(std::make_unique<juce::AudioParameterFloat>("sep", "SEPARATE", 0, 1.0, 0.5));
	layout.add(std::make_unique<juce::AudioParameterFloat>("fdbk1", "FEEDBACK1", 0, 1.0, 0.0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("fdbk2", "FEEDBACK2", 0, 1.0, 0.0));

	layout.add(std::make_unique<juce::AudioParameterFloat>("dry", "DRY", 0, 1.0, 0.25));
	layout.add(std::make_unique<juce::AudioParameterFloat>("wet", "WET", 0, 1.0, 0.75));

	return layout;
}

FDNTezAudioProcessor::~FDNTezAudioProcessor()
{
}

//==============================================================================
const juce::String FDNTezAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool FDNTezAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool FDNTezAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool FDNTezAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double FDNTezAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int FDNTezAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int FDNTezAudioProcessor::getCurrentProgram()
{
	return 0;
}

void FDNTezAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String FDNTezAudioProcessor::getProgramName(int index)
{
	return "";
}

void FDNTezAudioProcessor::changeProgramName(int index, const juce::String& newName)
{

}

//==============================================================================
void FDNTezAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
	FDNInit(&revb);
	//FDNApplyHadamardMatrix(&revb);
	//FDNApplyRandomMatrix(&revb);
	FDNApplyHouseholderMatrix(&revb);
}

void FDNTezAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FDNTezAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif


void FDNTezAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

	auto numSamples = buffer.getNumSamples();
	float* wavbufl = buffer.getWritePointer(0);
	float* wavbufr = buffer.getWritePointer(1);
	const float* recbufl = buffer.getReadPointer(0);
	const float* recbufr = buffer.getReadPointer(1);

	float SampleRate = getSampleRate();

	float sizel = *Params.getRawParameterValue("sizel");
	float sizer = *Params.getRawParameterValue("sizer");
	float sizmode = *Params.getRawParameterValue("sizmode");
	float sep = *Params.getRawParameterValue("sep");
	float fdbk1 = *Params.getRawParameterValue("fdbk1");
	float fdbk2 = *Params.getRawParameterValue("fdbk2");
	float dry = *Params.getRawParameterValue("dry");
	float wet = *Params.getRawParameterValue("wet");

	SetFDNRoomSize(&revb, sizel, sizer, sizmode * sizmode);
	SetFDNSeparate(&revb, sep);
	SetFDNDryWet(&revb, dry, wet);
	SetFDNFeedback(&revb, fdbk1 * fdbk1 * 8.0, fdbk2 * fdbk2);

	for (int i = 0; i < numSamples; ++i)
	{
		float datl = recbufl[i];
		float datr = recbufr[i];

		float outl = 0, outr = 0;

		StereoFloat in;
		in.l = datl;
		in.r = datr;
		StereoFloat out = FDNProc(&revb, in);
		outl = out.l;
		outr = out.r;

		wavbufl[i] = outl;
		wavbufr[i] = outr;
	}
}

//==============================================================================
bool FDNTezAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FDNTezAudioProcessor::createEditor()
{
	return new FDNTezAudioProcessorEditor(*this);

	//return new juce::GenericAudioProcessorEditor(*this);//自动绘制(调试)
}

//==============================================================================

void FDNTezAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
	auto state = Params.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void FDNTezAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
	{
		if (xmlState->hasTagName(Params.state.getType()))
		{
			Params.replaceState(juce::ValueTree::fromXml(*xmlState));
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new FDNTezAudioProcessor();
}
