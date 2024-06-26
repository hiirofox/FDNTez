/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FDNTezAudioProcessorEditor::FDNTezAudioProcessorEditor(FDNTezAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setSize(64 * 6, 64 * 2 + 64);

	S_SIZEL.setText("SIZE-L");
	S_SIZER.setText("SIZE-R");
	S_MODE.setText("SIZEMODE");
	S_SEP.setText("SEP");
	S_FDBK.setText("FDBK");
	S_DRY.setText("DRY");
	S_WET.setText("WET");

	S_SIZEL.ParamLink(audioProcessor.GetParams(), "sizel");
	S_SIZER.ParamLink(audioProcessor.GetParams(), "sizer");
	S_MODE.ParamLink(audioProcessor.GetParams(), "sizmode");
	S_SEP.ParamLink(audioProcessor.GetParams(), "sep");
	S_FDBK.ParamLink(audioProcessor.GetParams(), "fdbk");
	S_DRY.ParamLink(audioProcessor.GetParams(), "dry");
	S_WET.ParamLink(audioProcessor.GetParams(), "wet");

	addAndMakeVisible(S_SIZEL);
	addAndMakeVisible(S_SIZER);
	addAndMakeVisible(S_MODE);
	addAndMakeVisible(S_SEP);
	addAndMakeVisible(S_FDBK);
	addAndMakeVisible(S_DRY);
	addAndMakeVisible(S_WET);
}

FDNTezAudioProcessorEditor::~FDNTezAudioProcessorEditor()
{
}

//==============================================================================
void FDNTezAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(juce::Colour(0x00, 0x00, 0x00));

	//g.setColour (juce::Colours::white);
	//g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

	g.setFont(juce::Font("FIXEDSYS", 16.0, 1));
	g.setColour(juce::Colour(0x77, 0x77, 0x77));
	//g.drawText("Phasex V1.0 (L-MODEL) Hiirofox&Ristekv", 0, 120 + 64 * 1 - 16, 64 * 5, 16, 0);
}

void FDNTezAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
	S_SIZEL.setPos(64 * 0 + 64, 64 * 1);
	S_SIZER.setPos(64 * 0 + 64, 64 * 2);
	S_MODE.setPos(64 * 1 + 64, 64 * 1);
	S_SEP.setPos(64 * 1 + 64, 64 * 2);
	S_FDBK.setPos(64 * 2 + 64, 64 * 1);
	S_DRY.setPos(64 * 4 + 64, 64 * 1);
	S_WET.setPos(64 * 4 + 64, 64 * 2);
}
