/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EQAudioProcessorEditor::EQAudioProcessorEditor(EQAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(lowCutSlopeSlider);
    addAndMakeVisible(highCutSlopeSlider);
    addAndMakeVisible(highCutFreqSlider);
    addAndMakeVisible(lowCutFreqSlider);
    addAndMakeVisible(peakQualitySlider);
    addAndMakeVisible(peakGainSlider);
    addAndMakeVisible(peakFreqSlider);
    addAndMakeVisible(lowCutSlopeSlider);
    addAndMakeVisible(highCutSlopeSlider);
    
    setSize(1200, 900);
}

EQAudioProcessorEditor::~EQAudioProcessorEditor()
{
}

//==============================================================================
void EQAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    
}

void EQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);
    auto freqAreaBounds = bounds.removeFromTop(bounds.getHeight() * 0.5);

    auto lowCutArea = freqAreaBounds.removeFromLeft(bounds.getWidth() * 0.5);
    auto highCutArea = freqAreaBounds;

    auto lowCutFreqArea = lowCutArea.removeFromLeft(lowCutArea.getWidth() * 0.5);
    auto lowCutSlopeArea = lowCutArea;

    auto highCutFreqArea = highCutArea.removeFromLeft(highCutArea.getWidth() * 0.5);
    auto highCutSlopeArea = highCutArea;

    auto peakQualityArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto peakFreqArea = bounds.removeFromRight(bounds.getWidth() * 0.5);
    auto peakGainArea = bounds;


    lowCutFreqSlider.setBounds(lowCutFreqArea);
    highCutFreqSlider.setBounds(highCutFreqArea); 

    lowCutSlopeSlider.setBounds(lowCutSlopeArea);
    highCutSlopeSlider.setBounds(highCutSlopeArea);
    
    peakFreqSlider.setBounds(peakFreqArea);
    peakGainSlider.setBounds(peakGainArea);
    peakQualitySlider.setBounds(peakQualityArea);

}

