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

    juce::dsp::ProcessSpec shem;

    shem.maximumBlockSize = samplesPerBlock;

    shem.numChannels = 1;

    shem.sampleRate = sampleRate;

    leftChain.prepare(shem);
    rightChain.prepare(shem);

    auto getchainSettings = getchainsettings(apvts);

    // IIR or Infinite-Duration Impulse Response Filters uses a feedback mechanism where the previous output,
    //in conjunction with the present and past input,
    //is given as the present input.

    //coefficients for the peak filter:

    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, getchainSettings.peakFreq, getchainSettings.peakQuality, juce::Decibels::decibelsToGain(getchainSettings.peakGain));

    //set the filter's coefficients accordingly:
    //IIR functions return instances on the heap, rather than in the actual audio buffer.
    // To access these, you must dereference them

    *leftChain.get<Peak>().coefficients = *peakCoefficients; //the star is used to deref.
    *rightChain.get<Peak>().coefficients = *peakCoefficients;


    // every 2 orders creates an IIRHighPass / LowCut filter
    // so for x iterations of the filter we need 2x filters
    //as the first slope choice starts from 0, we need 2x+1 filters
    
    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(getchainSettings.lowCutFreq, getSampleRate(), (getchainSettings.lowCutSlope + 1) * 2);

    auto& RightLowCut = rightChain.get<LowCut>();
    auto& leftLowCut = leftChain.get<LowCut>();


    RightLowCut.setBypassed<0>(true);
    RightLowCut.setBypassed<1>(true);
    RightLowCut.setBypassed<2>(true);
    RightLowCut.setBypassed<3>(true);

    switch (getchainSettings.lowCutSlope)
    {
        case Slope4:
        {

            *RightLowCut.get<3>().coefficients = *lowCutCoefficients[3];
            RightLowCut.setBypassed<3>(false);
            [[fallthrough]]; // fallthrough is explicit
        }

        case Slope3:
        {

            *RightLowCut.get<2>().coefficients = *lowCutCoefficients[2];
            RightLowCut.setBypassed<2>(false);
            [[fallthrough]]; // fallthrough is explicit
        }
        case Slope2:
        {

            *RightLowCut.get<1>().coefficients = *lowCutCoefficients[1];
            RightLowCut.setBypassed<1>(false);
            [[fallthrough]]; // fallthrough is explicit
        }
        case Slope1:
        {
            *RightLowCut.get<0>().coefficients = *lowCutCoefficients[0];
            RightLowCut.setBypassed<0>(false);
            [[fallthrough]]; // fallthrough is explicit
        }
        

    }

    leftLowCut.setBypassed<0>(true);
    leftLowCut.setBypassed<1>(true);
    leftLowCut.setBypassed<2>(true);
    leftLowCut.setBypassed<3>(true);
    
    switch (getchainSettings.lowCutSlope)
    {
        case Slope4:
        {

            *leftLowCut.get<3>().coefficients = *lowCutCoefficients[3];
            leftLowCut.setBypassed<3>(false);
            [[fallthrough]]; // fallthrough is explicit
        }

        case Slope3:
        {

            *leftLowCut.get<2>().coefficients = *lowCutCoefficients[2];
            leftLowCut.setBypassed<2>(false);
            [[fallthrough]]; // fallthrough is explicit
        }
        case Slope2:
        {

            *leftLowCut.get<1>().coefficients = *lowCutCoefficients[1];
            leftLowCut.setBypassed<1>(false);
            [[fallthrough]]; // fallthrough is explicit
        }
        case Slope1:
        {
            *leftLowCut.get<0>().coefficients = *lowCutCoefficients[0];
            leftLowCut.setBypassed<0>(false);
            [[fallthrough]]; // fallthrough is explicit
        }
        

    }

    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(getchainSettings.highCutFreq, getSampleRate(), (getchainSettings.highCutSlope + 1) * 2);

    auto& RightHighCut = rightChain.get<HighCut>();
    auto& LeftHighCut = leftChain.get<HighCut>();

    RightHighCut.setBypassed<0>(true);
    RightHighCut.setBypassed<1>(true);
    RightHighCut.setBypassed<2>(true);
    RightHighCut.setBypassed<3>(true);
    
    switch (getchainSettings.highCutSlope)
    {
    case Slope4:
    {

        *RightHighCut.get<3>().coefficients = *highCutCoefficients[3];
        RightHighCut.setBypassed<3>(false);
        [[fallthrough]]; // fallthrough is explicit
    }

    case Slope3:
    {

        *RightHighCut.get<2>().coefficients = *highCutCoefficients[2];
        RightHighCut.setBypassed<2>(false);
        [[fallthrough]]; // fallthrough is explicit
    }
    case Slope2:
    {

        *RightHighCut.get<1>().coefficients = *highCutCoefficients[1];
        RightHighCut.setBypassed<1>(false);
        [[fallthrough]]; // fallthrough is explicit
    }
    case Slope1:
    {
        *RightHighCut.get<0>().coefficients = *highCutCoefficients[0];
        RightHighCut.setBypassed<0>(false);
        [[fallthrough]]; // fallthrough is explicit
    }


    }

    LeftHighCut.setBypassed<0>(true);
    LeftHighCut.setBypassed<1>(true);
    LeftHighCut.setBypassed<2>(true);
    RightHighCut.setBypassed<3>(true);

    switch (getchainSettings.highCutSlope)
    {
    case Slope4:
    {

        *LeftHighCut.get<3>().coefficients = *highCutCoefficients[3];
        LeftHighCut.setBypassed<3>(false);
        [[fallthrough]]; // fallthrough is explicit
    }

    case Slope3:
    {

        *LeftHighCut.get<2>().coefficients = *highCutCoefficients[2];
        LeftHighCut.setBypassed<2>(false);
        [[fallthrough]]; // fallthrough is explicit
    }
    case Slope2:
    {

        *LeftHighCut.get<1>().coefficients = *highCutCoefficients[1];
        LeftHighCut.setBypassed<1>(false);
        [[fallthrough]]; // fallthrough is explicit
    }
    case Slope1:
    {
        *LeftHighCut.get<0>().coefficients = *highCutCoefficients[0];
        LeftHighCut.setBypassed<0>(false);
        [[fallthrough]]; // fallthrough is explicit
    }


    }



}

void EQAudioProcessor::releaseResources()
{
    // When playback stops,B you can use this as an opportunity to free up any
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

    *leftChain.get<Peak>().coefficients = *peakCoefficients; //as therse are references, they point to the adresses of variables.
    *rightChain.get<Peak>().coefficients = *peakCoefficients;// to get the data in these variables, we must dereference them.

    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(getchainSettings.lowCutFreq, getSampleRate(), (getchainSettings.lowCutSlope + 1) * 2);

    auto& RightLowCut = rightChain.get<LowCut>();

    RightLowCut.setBypassed<0>(true);
    RightLowCut.setBypassed<1>(true);
    RightLowCut.setBypassed<2>(true);
    RightLowCut.setBypassed<3>(true);

    switch (getchainSettings.lowCutSlope)
    {
        case Slope4:
        {

            *RightLowCut.get<3>().coefficients = *lowCutCoefficients[3];
            RightLowCut.setBypassed<3>(false);
            [[fallthrough]]; // fallthrough is explicit
        }

        case Slope3:
        {

            *RightLowCut.get<2>().coefficients = *lowCutCoefficients[2];
            RightLowCut.setBypassed<2>(false);
            [[fallthrough]]; // fallthrough is explicit
        }
        case Slope2:
        {

            *RightLowCut.get<1>().coefficients = *lowCutCoefficients[1];
            RightLowCut.setBypassed<1>(false);
            [[fallthrough]]; // fallthrough is explicit
        }
        case Slope1:
        {
            *RightLowCut.get<0>().coefficients = *lowCutCoefficients[0];
            RightLowCut.setBypassed<0>(false);
            [[fallthrough]]; // fallthrough is explicit
        }
        

    }
   
    auto& leftLowCut = leftChain.get<LowCut>();

    leftLowCut.setBypassed<0>(true);
    leftLowCut.setBypassed<1>(true);
    leftLowCut.setBypassed<2>(true);
    leftLowCut.setBypassed<3>(true);
    switch (getchainSettings.lowCutSlope)
    {
        case Slope4:
        {

            *leftLowCut.get<3>().coefficients = *lowCutCoefficients[3];
            leftLowCut.setBypassed<3>(false);
            [[fallthrough]]; // fallthrough is explicit
        }

        case Slope3:
        {

            *leftLowCut.get<2>().coefficients = *lowCutCoefficients[2];
            leftLowCut.setBypassed<2>(false);
            [[fallthrough]]; // fallthrough is explicit
        }
        case Slope2:
        {

            *leftLowCut.get<1>().coefficients = *lowCutCoefficients[1];
            leftLowCut.setBypassed<1>(false);
            [[fallthrough]]; // fallthrough is explicit
        }
        case Slope1:
        {
            *leftLowCut.get<0>().coefficients = *lowCutCoefficients[0];
            leftLowCut.setBypassed<0>(false);
            [[fallthrough]]; // fallthrough is explicit
        }
        

    }
    
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(getchainSettings.highCutFreq, getSampleRate(), (getchainSettings.highCutSlope + 1) * 2);

    auto& RightHighCut = rightChain.get<HighCut>();
    auto& LeftHighCut = leftChain.get<HighCut>();

    RightHighCut.setBypassed<0>(true);
    RightHighCut.setBypassed<1>(true);
    RightHighCut.setBypassed<2>(true);
    RightHighCut.setBypassed<3>(true);

    switch (getchainSettings.highCutSlope)
    {
    case Slope4:
    {

        *RightHighCut.get<3>().coefficients = *highCutCoefficients[3];
        RightHighCut.setBypassed<3>(false);
        [[fallthrough]]; // fallthrough is explicit
    }

    case Slope3:
    {

        *RightHighCut.get<2>().coefficients = *highCutCoefficients[2];
        RightHighCut.setBypassed<2>(false);
        [[fallthrough]]; // fallthrough is explicit
    }
    case Slope2:
    {

        *RightHighCut.get<1>().coefficients = *highCutCoefficients[1];
        RightHighCut.setBypassed<1>(false);
        [[fallthrough]]; // fallthrough is explicit
    }
    case Slope1:
    {
        *RightHighCut.get<0>().coefficients = *highCutCoefficients[0];
        RightHighCut.setBypassed<0>(false);
        [[fallthrough]]; // fallthrough is explicit
    }


    }

    LeftHighCut.setBypassed<0>(true);
    LeftHighCut.setBypassed<1>(true);
    LeftHighCut.setBypassed<2>(true);
    RightHighCut.setBypassed<3>(true);

    switch (getchainSettings.highCutSlope)
    {
    case Slope4:
    {

        *LeftHighCut.get<3>().coefficients = *highCutCoefficients[3];
        LeftHighCut.setBypassed<3>(false);
        [[fallthrough]]; // fallthrough is explicit
    }

    case Slope3:
    {

        *LeftHighCut.get<2>().coefficients = *highCutCoefficients[2];
        LeftHighCut.setBypassed<2>(false);
        [[fallthrough]]; // fallthrough is explicit
    }
    case Slope2:
    {

        *LeftHighCut.get<1>().coefficients = *highCutCoefficients[1];
        LeftHighCut.setBypassed<1>(false);
        [[fallthrough]]; // fallthrough is explicit
    }
    case Slope1:
    {
        *LeftHighCut.get<0>().coefficients = *highCutCoefficients[0];
        LeftHighCut.setBypassed<0>(false);
        [[fallthrough]]; // fallthrough is explicit
    }


    }
    
    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContextReplacing(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContextReplacing(rightBlock);

    leftChain.process(leftContextReplacing);
    rightChain.process(rightContextReplacing);


    
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
    chainsettings settings; // denormalises the values of each parameter so that we can get the real-world values

    settings.lowCutFreq = apvts.getRawParameterValue("lowcutFreq")->load();
    settings.highCutFreq = apvts.getRawParameterValue("highcutFreq")->load();
    settings.peakFreq = apvts.getRawParameterValue("PeakFreq")->load();
    settings.peakGain = apvts.getRawParameterValue("peakGain")->load();
    settings.peakQuality = apvts.getRawParameterValue("peakQuality")->load();
    settings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("lowcutSlope")->load()); 
    settings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue("highcutSlope")->load());

    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout EQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    //RangeStart,RangeEnd,Interval, SkewFactor

    //LowCut Frequency

    layout.add(std::make_unique<juce::AudioParameterFloat>("lowcutFreq", "LowCut Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 20.f));

    //HighCut Frequency

    layout.add(std::make_unique<juce::AudioParameterFloat>("highcutFreq", "HighCut Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 20000.f));

    //Parametric Peak

    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakFreq", "Peak Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f), 750.f));

    //Peak Gain

    layout.add(std::make_unique<juce::AudioParameterFloat>("peakGain", "Peak Gain", juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f), 0.0f));

    //Peak Quality (How wide / narrow the peak is)

    layout.add(std::make_unique<juce::AudioParameterFloat>("peakQuality", "Peak Quality", juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f), 1.f));

    // stringArray dropdown table

    juce::StringArray stringArray;
    for (int i = 0; i < 4; ++i) // I IN RANGE 1 TO 4, INTERVAL OF 1
    {
        juce::String str; str << (12 + i * 12); str << " dB/Oct"; stringArray.add(str);
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
}