/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct RotarySlider : juce::Slider
{
    RotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    {


    }
};

//==============================================================================
/**
*/
class EQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    EQAudioProcessorEditor(EQAudioProcessor&);
    ~EQAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EQAudioProcessor& audioProcessor;

    RotarySlider peakFreqSlider;
    RotarySlider peakGainSlider;
    RotarySlider peakQualitySlider;
    RotarySlider lowCutFreqSlider;
    RotarySlider highCutFreqSlider;
    RotarySlider lowCutSlopeSlider;
    RotarySlider highCutSlopeSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQAudioProcessorEditor)
}; 