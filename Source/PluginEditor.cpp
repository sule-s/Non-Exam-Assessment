/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include<iostream> 
#include<list> 


//==============================================================================
EQAudioProcessorEditor::EQAudioProcessorEditor(EQAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    peakFreqAttachment(audioProcessor.apvts, "PeakFreq", peakFreqSlider),
    peakGainAttachment(audioProcessor.apvts, "peakGain", peakGainSlider),
    peakQualityAttachment(audioProcessor.apvts, "peakQuality", peakQualitySlider),
    lowCutSlopeAttachment(audioProcessor.apvts, "lowcutSlope", lowCutSlopeSlider),
    highCutSlopeAttachment(audioProcessor.apvts, "highcutSlope", highCutSlopeSlider),
    lowCutFreqAttachment(audioProcessor.apvts, "lowcutFreq", lowCutFreqSlider),
    highCutFreqAttachment(audioProcessor.apvts, "highcutFreq", highCutFreqSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

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
 
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);
    auto freqAreaBounds = bounds.removeFromTop(bounds.getHeight() * 0.5);

    auto lowCutArea = freqAreaBounds.removeFromLeft(bounds.getWidth() * 0.5);
    auto highCutArea = freqAreaBounds;




    auto lowCutFreqArea = lowCutArea.removeFromLeft(lowCutArea.getWidth() * 0.5);
    auto lowCutFreqArea1 = lowCutFreqArea.removeFromLeft(lowCutFreqArea.getWidth() * 0.95);
    auto lowCutFreqArea2 = lowCutFreqArea1.removeFromRight(lowCutFreqArea1.getWidth() * 0.9025);
    auto lowCutFreqArea3 = lowCutFreqArea2.removeFromTop(lowCutFreqArea2.getWidth() * 0.95);
    auto lowCutFreqArea4 = lowCutFreqArea3.removeFromBottom(lowCutFreqArea3.getWidth() * 0.9025);



    auto lowCutSlopeArea = lowCutArea;
    auto lowCutSlopeArea1 = lowCutSlopeArea.removeFromLeft(lowCutSlopeArea.getWidth() * 0.95);
    auto lowCutSlopeArea2 = lowCutSlopeArea1.removeFromRight(lowCutSlopeArea1.getWidth() * 0.9025);
    auto lowCutSlopeArea3 = lowCutSlopeArea2.removeFromTop(lowCutSlopeArea2.getWidth() * 0.95);
    auto lowCutSlopeArea4 = lowCutSlopeArea3.removeFromBottom(lowCutSlopeArea3.getWidth() * 0.9025);


    auto highCutFreqArea = highCutArea.removeFromLeft(highCutArea.getWidth() * 0.5);
    auto highCutFreqArea1 = highCutFreqArea.removeFromLeft(highCutFreqArea.getWidth() * 0.95);
    auto highCutFreqArea2 = highCutFreqArea1.removeFromRight(highCutFreqArea1.getWidth() * 0.9025);
    auto highCutFreqArea3 = highCutFreqArea2.removeFromTop(highCutFreqArea2.getWidth() * 0.95);
    auto highCutFreqArea4 = highCutFreqArea3.removeFromBottom(highCutFreqArea3.getWidth() * 0.9025);

    auto highCutSlopeArea = highCutArea;
    auto highCutSlopeArea1 = highCutSlopeArea.removeFromLeft(highCutSlopeArea.getWidth() * 0.95);
    auto highCutSlopeArea2 = highCutSlopeArea1.removeFromRight(highCutSlopeArea1.getWidth() * 0.9025);
    auto highCutSlopeArea3 = highCutSlopeArea2.removeFromTop(highCutSlopeArea2.getWidth() * 0.95);
    auto highCutSlopeArea4 = highCutSlopeArea3.removeFromBottom(highCutSlopeArea3.getWidth() * 0.9025);


    auto peakQualityArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto peakQualityArea1 = peakQualityArea.removeFromLeft(peakQualityArea.getWidth() * 0.95);
    auto peakQualityArea2 = peakQualityArea1.removeFromRight(peakQualityArea1.getWidth() * 0.9025);
    auto peakQualityArea3 = peakQualityArea2.removeFromTop(peakQualityArea2.getWidth() * 0.95);
    auto peakQualityArea4 = peakQualityArea3.removeFromBottom(peakQualityArea3.getWidth() * 0.9025);


    auto peakFreqArea = bounds.removeFromRight(bounds.getWidth() * 0.5);
    auto peakFreqArea1 = peakFreqArea.removeFromLeft(peakFreqArea.getWidth() * 0.95);
    auto peakFreqArea2 = peakFreqArea1.removeFromRight(peakFreqArea1.getWidth() * 0.9025);
    auto peakFreqArea3 = peakFreqArea2.removeFromTop(peakFreqArea2.getWidth() * 0.95);
    auto peakFreqArea4 = peakFreqArea3.removeFromBottom(peakFreqArea3.getWidth() * 0.9025);

    auto peakGainArea = bounds;
    auto peakGainArea1 = peakGainArea.removeFromLeft(peakGainArea.getWidth() * 0.95);
    auto peakGainArea2 = peakGainArea1.removeFromRight(peakGainArea1.getWidth() * 0.9025);
    auto peakGainArea3 = peakGainArea2.removeFromTop(peakGainArea2.getWidth() * 0.95);
    auto peakGainArea4 = (peakGainArea3.removeFromBottom(peakGainArea3.getWidth() * 0.9025));

    lowCutFreqSlider.setBounds(lowCutFreqArea4);
    highCutFreqSlider.setBounds(highCutFreqArea4);

    lowCutSlopeSlider.setBounds(lowCutSlopeArea4);
    highCutSlopeSlider.setBounds(highCutSlopeArea4);

    peakFreqSlider.setBounds(peakFreqArea4);
    peakGainSlider.setBounds(peakGainArea4);
    peakQualitySlider.setBounds(peakQualityArea4);
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Low Cut Frequency",lowCutFreqArea4, juce::Justification::centred, 1);
    g.drawFittedText("High Cut Frequency", highCutFreqArea4, juce::Justification::centred, 1);
    g.drawFittedText("Low Cut Slope", lowCutSlopeArea4, juce::Justification::centred, 1);
    g.drawFittedText("High Cut Slope", highCutSlopeArea4, juce::Justification::centred, 1);
    g.drawFittedText("Peak Frequency", peakFreqArea4, juce::Justification::centred, 1);
    g.drawFittedText("Peak Gain", peakGainArea4, juce::Justification::centred, 1);
    g.drawFittedText("Peak Quality", peakQualityArea4, juce::Justification::centred, 1);
    g.setFont(45.0f);
    g.setColour(juce::Colours::lightblue);
    g.drawFittedText("3 Band Parametric EQ", responseArea, juce::Justification::centred, 1);
    
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
    auto lowCutFreqArea1 = lowCutFreqArea.removeFromLeft(lowCutFreqArea.getWidth() * 0.95);
    auto lowCutFreqArea2 = lowCutFreqArea1.removeFromRight(lowCutFreqArea1.getWidth() * 0.9025);
    auto lowCutFreqArea3 = lowCutFreqArea2.removeFromTop(lowCutFreqArea2.getWidth() * 0.95);
    auto lowCutFreqArea4 = lowCutFreqArea3.removeFromBottom(lowCutFreqArea3.getWidth() * 0.9025);



    auto lowCutSlopeArea = lowCutArea;
    auto lowCutSlopeArea1 = lowCutSlopeArea.removeFromLeft(lowCutSlopeArea.getWidth() * 0.95);
    auto lowCutSlopeArea2 = lowCutSlopeArea1.removeFromRight(lowCutSlopeArea1.getWidth() * 0.9025);
    auto lowCutSlopeArea3 = lowCutSlopeArea2.removeFromTop(lowCutSlopeArea2.getWidth() * 0.95);
    auto lowCutSlopeArea4 = lowCutSlopeArea3.removeFromBottom(lowCutSlopeArea3.getWidth() * 0.9025);


    auto highCutFreqArea = highCutArea.removeFromLeft(highCutArea.getWidth() * 0.5);
    auto highCutFreqArea1 = highCutFreqArea.removeFromLeft(highCutFreqArea.getWidth() * 0.95);
    auto highCutFreqArea2 = highCutFreqArea1.removeFromRight(highCutFreqArea1.getWidth() * 0.9025);
    auto highCutFreqArea3 = highCutFreqArea2.removeFromTop(highCutFreqArea2.getWidth() * 0.95);
    auto highCutFreqArea4 = highCutFreqArea3.removeFromBottom(highCutFreqArea3.getWidth() * 0.9025);

    auto highCutSlopeArea = highCutArea;
    auto highCutSlopeArea1 = highCutSlopeArea.removeFromLeft(highCutSlopeArea.getWidth() * 0.95);
    auto highCutSlopeArea2 = highCutSlopeArea1.removeFromRight(highCutSlopeArea1.getWidth() * 0.9025);
    auto highCutSlopeArea3 = highCutSlopeArea2.removeFromTop(highCutSlopeArea2.getWidth() * 0.95);
    auto highCutSlopeArea4 = highCutSlopeArea3.removeFromBottom(highCutSlopeArea3.getWidth() * 0.9025);


    auto peakQualityArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto peakQualityArea1 = peakQualityArea.removeFromLeft(peakQualityArea.getWidth() * 0.95);
    auto peakQualityArea2 = peakQualityArea1.removeFromRight(peakQualityArea1.getWidth() * 0.9025);
    auto peakQualityArea3 = peakQualityArea2.removeFromTop(peakQualityArea2.getWidth() * 0.95);
    auto peakQualityArea4 = peakQualityArea3.removeFromBottom(peakQualityArea3.getWidth() * 0.9025);


    auto peakFreqArea = bounds.removeFromRight(bounds.getWidth() * 0.5);
    auto peakFreqArea1 = peakFreqArea.removeFromLeft(peakFreqArea.getWidth() * 0.95);
    auto peakFreqArea2 = peakFreqArea1.removeFromRight(peakFreqArea1.getWidth() * 0.9025);
    auto peakFreqArea3 = peakFreqArea2.removeFromTop(peakFreqArea2.getWidth() * 0.95);
    auto peakFreqArea4 = peakFreqArea3.removeFromBottom(peakFreqArea3.getWidth() * 0.9025);

    auto peakGainArea = bounds;
    auto peakGainArea1 = peakGainArea.removeFromLeft(peakGainArea.getWidth() * 0.95);
    auto peakGainArea2 = peakGainArea1.removeFromRight(peakGainArea1.getWidth() * 0.9025);
    auto peakGainArea3 = peakGainArea2.removeFromTop(peakGainArea2.getWidth() * 0.95);
    auto peakGainArea4 = (peakGainArea3.removeFromBottom(peakGainArea3.getWidth() * 0.9025));
    
    lowCutFreqSlider.setBounds(lowCutFreqArea4);
    highCutFreqSlider.setBounds(highCutFreqArea4);

    lowCutSlopeSlider.setBounds(lowCutSlopeArea4);
    highCutSlopeSlider.setBounds(highCutSlopeArea4);

    peakFreqSlider.setBounds(peakFreqArea4);
    peakGainSlider.setBounds(peakGainArea4);
    peakQualitySlider.setBounds(peakQualityArea4);
    
}
