/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//creating a structure so that the apvts can pull these values every time it is called, rather than having to write them out over and over.
struct ChainSettings
{
    int lowcutSlope{ 0 }, highcutSlope{ 0 };
    float peakFreq{ 0 }, peakGain{ 0 }, peakQuality{ 1.f };
    float lowcutFreq{ 0 }, highcutFreq{ 0 };
    
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);
//==============================================================================
/**
*/
class EQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    EQAudioProcessor();
    ~EQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override; 

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //initialising the apvts so that a GUI can be used for with sliders, knobs etc. 

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };
    
    //creating variables for QoL 
private:
    using Filter = juce::dsp::IIR::Filter<float>; //slope of cut filters are multiples of 12dB/Oct and filters defaults at 12dB/Oct, but we want up to 48 dB/Oct
                                                  //COULD USE STATEVARIABLEFILTER, but allows for single channel manuipulation
    using Cut = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>; // 12 dB/Oct * 4 = 48dB/Oct 

    using StereoChain = juce::dsp::ProcessorChain < Cut, Filter, Cut>; // initiating LowCut, Parametric Band, High Cut

    StereoChain leftChain, rightChain; //dsp defaults as mono instead of stereo, so creating two channels results in stereo sound 
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EQAudioProcessor)
};
