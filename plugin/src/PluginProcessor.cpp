#include "QSynthi2/PluginProcessor.h"
#include "QSynthi2/PluginEditor.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"
#include <iostream>

#include "QSynthi2/Data.h"

// shared content instance (backend & frontend communication)
Data sharedData;

// simple cout logger
//
class SimpleLogger final : public juce::Logger {
public:
    ~SimpleLogger() override = default;
    SimpleLogger() = default;
protected:
    void logMessage(const juce::String &message) override {
        std::cout << message << std::endl;
    }
};

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{

    // set logger to cout if current logger does not exist
    if (juce::Logger::getCurrentLogger() == nullptr)
        juce::Logger::setCurrentLogger(new SimpleLogger());

    sharedData.parameters = new ParameterCollection();
    sharedData.parameters->connectTo(*this);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    delete sharedData.parameters;
    sharedData.parameters = nullptr;
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
    // Todo: add release time here
}



int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}



int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}



void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}



const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}



void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}



void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    if (sampleRate > 0) {
        sharedData.parameters->prepareToPlay(static_cast<Decimal>(sampleRate), samplesPerBlock);
        audioProcessor.prepareToPlay(sampleRate, samplesPerBlock);
    }
}



void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}



bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}



void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = 0; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    sharedData.parameters->processBlock();
    audioProcessor.processBlock(buffer, midiMessages);


}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor() {
    // return new juce::GenericAudioProcessorEditor(*this);
    return new AudioPluginAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    sharedData.parameters->getStateInformation(destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    sharedData.parameters->setStateInformation(data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new AudioPluginAudioProcessor();
}