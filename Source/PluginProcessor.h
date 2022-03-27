/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin processor.
  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>

//creating a structure so that the apvts can pull these values every time it is called, rather than having to write them out over and over.
struct chainsettings
{
    float peakFreq{ 0 }, peakGain{ 0 }, peakQuality{ 1.f };
    float lowCutFreq{ 0 }, highCutFreq{ 0 };
    int lowCutSlope{ 0 }, highCutSlope{ 0 };
};

chainsettings getchainsettings(juce::AudioProcessorValueTreeState& apvts);

//==============================================================================
/**
*/
class EQAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    EQAudioProcessor();
    ~EQAudioProcessor() override;
    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;
    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

    //slope of cut filters are multiples of 12dB/Oct and filters defaults at 12dB/Oct, but we want up to 48 dB/Oct

private:
    using Filter = juce::dsp::IIR::Filter<float>;

    using VariableCut = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>; // 12 dB/Oct * 4 = 48dB/Oct

    using SingleChain = juce::dsp::ProcessorChain<VariableCut, Filter, VariableCut>;
    //dsp defaults as mono instead of stereo, so creating a left and right channel
    //to play concurrently will result in stereo sound

    SingleChain leftChain, rightChain;


    //used to store a set of constants that can be called for the filter's coefficients.
    enum chainposition
    {
        LowCut, Peak, HighCut
    };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQAudioProcessor)
};