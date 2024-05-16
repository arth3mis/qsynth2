//
// Created by Jannis Müller on 15.05.24.
//
#include "QSynthi2/AudioProcessing/AJAudioProcessor.h"
#include "QSynthi2/Data.h"

extern Data sharedData;


AJAudioProcessor::AJAudioProcessor() {
    synth.setVoiceStealingEnabled (false); // TODO: Parameter
    for (auto i = 0; i < 15; ++i)
        synth.addVoice (new Voice());
}

void AJAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    synth.setCurrentPlaybackSampleRate(sampleRate);

    juce::ignoreUnused (samplesPerBlock);

}

void AJAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
    sharedData.functionCallStopwatch.start();

    /*
     *     for (const auto &m : midiMessages) {
    const auto midiEvent = m.getMessage();
    const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());

    juce::Logger::writeToLog(midiEvent.getDescription());
    }
     */

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    sharedData.functionCallStopwatch.stop();
    sharedData.parameterStopwatch.print();
    sharedData.modulationStopwatch.print();
    sharedData.functionCallStopwatch.print();
    sharedData.simulationStopwatch.print();
}
