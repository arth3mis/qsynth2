#include "QSynthi2/PluginProcessor.h"
#include "QSynthi2/PluginEditor.h"
#include <iostream>

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{

    parameter = std::make_shared<Parameter>();
    parameter->connectTo(*this);

    // set logger to cout if current logger does not exist
    if (juce::Logger::getCurrentLogger() == nullptr)
        juce::Logger::setCurrentLogger(new SimpleLogger());

    parameter->addListener(parameter->testParameterID, [&](float x){
        juce::Logger::writeToLog(juce::String(x));
        floatVariable = x;
        doubleVariable = x;
    });

}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
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

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        juce::Logger::writeToLog(juce::String(buffer.getArrayOfWritePointers()[0][0]));

        juce::ignoreUnused (channelData);
        // ..do something to the data...
    }


    benchmarkCounter++;
    benchmarkCounter %= 42;

    if (benchmarkCounter == 0) {


        for (size_t benchmarkAmount = 80000; benchmarkAmount >= 625; benchmarkAmount /= 2) {
            auto start_time = std::chrono::high_resolution_clock::now();

            std::vector<double> samples = std::vector<double>(benchmarkSamples.size());
            float sum = 0;

            for (size_t j = 0; j < benchmarkAmount; j++) {
                for (size_t i = 0; i < benchmarkSamples.size(); i++) {
                    samples[i] = benchmarkSamples[i];


                    if (std::rand() > 0.5 * RAND_MAX) samples[i] *= floatVariable;
                    else samples[i] -= floatVariable;

                    if (std::abs(samples[i]) > 2e16) samples[i] = 1;
                    else if (std::abs(samples[i]) < 2e-16) samples[i] = 1;

                    sum += samples[i];
                }
            }


            auto end_time = std::chrono::high_resolution_clock::now();
            auto time = (end_time - start_time) / std::chrono::milliseconds(1);
            juce::Logger::writeToLog(
                    "iterations: " + juce::String(benchmarkAmount) + "\t Double vector: " + juce::String(time) +
                    "ms  \t sum: " + juce::String(sum));
            total_0 += time;






            auto d_start_time = std::chrono::high_resolution_clock::now();

            List<double> d_samples = List<double>(benchmarkSamples.size());


            for (size_t j = 0; j < benchmarkAmount; j++) {
                d_samples = d_samples.map<double>([&](double x) {
                    double y = 0;
                    if (std::rand() > 0.5 * RAND_MAX) y = x * doubleVariable;
                    else y = x - doubleVariable;

                    if (std::abs(y) > 2e16) y = 1;
                    else if (std::abs(y) < 2e-16) y = 1;
                    return y;
                });

            }

            double d_sum = List<double>::sumOf(d_samples);


            auto d_end_time = std::chrono::high_resolution_clock::now();
            auto time_1 = (d_end_time - d_start_time) / std::chrono::milliseconds(1);
            juce::Logger::writeToLog(
                    "iterations: " + juce::String(benchmarkAmount) + "\t Double list : " + juce::String(time_1) +
                    "ms \t sum: " + juce::String(d_sum));
            total_1 += time_1;






            auto start_time_2 = std::chrono::high_resolution_clock::now();

            List<double> samples_2 = List<double>(benchmarkSamples.size());


            for (size_t j = 0; j < benchmarkAmount; j++) {
                samples_2.mapMutable([&](double x) {
                    double y = 0;
                    if (std::rand() > 0.5 * RAND_MAX) y = x * doubleVariable;
                    else y = x - doubleVariable;

                    if (std::abs(y) > 2e16) y = 1;
                    else if (std::abs(y) < 2e-16) y = 1;
                    return y;
                });

            }

            double sum_2 = List<double>::sumOf(samples_2);


            auto end_time_2 = std::chrono::high_resolution_clock::now();
            auto time_2 = (end_time_2 - start_time_2) / std::chrono::milliseconds(1);
            juce::Logger::writeToLog(
                    "iterations: " + juce::String(benchmarkAmount) + "\t Double list mutable : " + juce::String(time_2) +
                    "ms \t sum: " + juce::String(sum_2));
            total_2 += time_2;

        }

        juce::Logger::writeToLog("TOTAL:");
        juce::Logger::writeToLog("    Double vector:         " + juce::String((100 * total_0 /std::min(total_0, std::min(total_1, total_2)))) + "%");
        juce::Logger::writeToLog("    Double list immutable: " + juce::String((100 * total_1 /std::min(total_0, std::min(total_1, total_2)))) + "%");
        juce::Logger::writeToLog("    Double list mutable:   " + juce::String((100 * total_2 /std::min(total_0, std::min(total_1, total_2)))) + "%");
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
    //return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    parameter->getStateInformation(destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    parameter->setStateInformation(data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
