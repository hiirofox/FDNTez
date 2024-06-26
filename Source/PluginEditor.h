/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ui\LM_slider.h"

//==============================================================================
/**
*/
class FDNTezAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
	FDNTezAudioProcessorEditor(FDNTezAudioProcessor&);
	~FDNTezAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	FDNTezAudioProcessor& audioProcessor;

	LMKnob S_SIZEL,S_SIZER,S_MODE;
	LMKnob S_SEP;
	LMKnob S_FDBK;
	LMKnob S_DRY, S_WET;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FDNTezAudioProcessorEditor)
};
