/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin processor.
  ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
EQAudioProcessor::EQAudioProcessor()
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
{
}
EQAudioProcessor::~EQAudioProcessor()
{
}
//==============================================================================
const juce::String EQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}
bool EQAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}
bool EQAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}
bool EQAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}
double EQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}
int EQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}
int EQAudioProcessor::getCurrentProgram()
{
    return 0;
}
void EQAudioProcessor::setCurrentProgram(int index)
{
}
const juce::String EQAudioProcessor::getProgramName(int index)
{
    return {};
}
void EQAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}
//==============================================================================
void EQAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // pass a process spec object to both the left and right chain, this will be passed to each link in the chain
    // this prepares each chain to be prepared for processing 

    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;

    spec.numChannels = 1;

    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    auto getchainSettings = getchainsettings(apvts);

    // IIR or Infinite-Duration Impulse Response Filters uses a feedback mechanism where the previous output,
    //in conjunction with the present and past input,
    //is given as the present input.

    //coefficients for the peak filter:

    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, getchainSettings.peakFreq, getchainSettings.peakQuality, juce::Decibels::decibelsToGain(getchainSettings.peakGain));

    //set the filter's coefficients accordingly:
    //IIR functions return instances on the heap, rather than in the actual audio buffer.
    // To access these, you must dereference them

    *leftChain.get<chainposition::Peak>().coefficients = *peakCoefficients; //the star is used to deref.
    *rightChain.get<chainposition::Peak>().coefficients = *peakCoefficients;
}

void EQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}
#ifndef JucePlugin_PreferredChannelConfigurations
bool EQAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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
void EQAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // this code must be implemented in processBlock AS WELL as prepareToPlay
    // so that the filter can be updated with the new coefficients when the user changes 
    // the desired frequency. 

    auto getchainSettings = getchainsettings(apvts);

    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), getchainSettings.peakFreq, getchainSettings.peakQuality, juce::Decibels::decibelsToGain(getchainSettings.peakGain));

    //A Processerchain needs processing context to be passed through it
    //in order to run audio via the links in the chain.
    //Processing contexts needs to be supplied by Audioblock instances. 

    *leftChain.get<chainposition::Peak>().coefficients = *peakCoefficients; //as therse are references, they point to the adresses of variables.
    *rightChain.get<chainposition::Peak>().coefficients = *peakCoefficients;// to get the data in these variables, we must dereference them.

    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);



}
//==============================================================================
bool EQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}
juce::AudioProcessorEditor* EQAudioProcessor::createEditor()
{
    //    return new EQAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}
//==============================================================================
void EQAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}
void EQAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

chainsettings getchainsettings(juce::AudioProcessorValueTreeState& apvts)
{
    chainsettings settings;

    settings.lowCutFreq = apvts.getRawParameterValue("lowcutFreq")->load();
    settings.highCutFreq = apvts.getRawParameterValue("highcutFreq")->load();
    settings.peakFreq = apvts.getRawParameterValue("PeakFreq")->load();
    settings.peakGain = apvts.getRawParameterValue("peakGain")->load();
    settings.peakQuality = apvts.getRawParameterValue("peakQuality")->load();
    settings.lowCutSlope = apvts.getRawParameterValue("lowcutSlope")->load();
    settings.highCutSlope = apvts.getRawParameterValue("highcutSlope")->load();

    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout EQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    //RangeStart,RangeEnd,Interval, SkewFactor

    //LowCut Frquency


    layout.add(std::make_unique<juce::AudioParameterFloat>("lowcutFreq", "LowCut Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 20.f));

    //HighCut Frquency

    layout.add(std::make_unique<juce::AudioParameterFloat>("highcutFreq", "HighCut Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 20000.f));

    //Parametric Peak

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakFreq", "Peak Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 750.f));

    //Peak Gain

    layout.add(std::make_unique<juce::AudioParameterFloat>("peakGain", "Peak Gain", juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f), 0.0f));

    //Peak Quality (How wide / narrow the peak is)

    layout.add(std::make_unique<juce::AudioParameterFloat>("peakQuality", "Peak Quality", juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f), 1.f));

    // stringArray dropdown table

    juce::StringArray stringArray;
    for (int i = 0; i < 4; ++i)
    {
        juce::String str;
        str << (12 + i * 12);
        str << " db/Oct";
        stringArray.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>("lowcutSlope", "LowCut Slope", stringArray, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("highcutSlope", "HighCut Slope", stringArray, 0));

    return layout;
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EQAudioProcessor();
}z