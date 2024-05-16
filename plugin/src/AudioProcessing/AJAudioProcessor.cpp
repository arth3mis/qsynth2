//
// Created by Jannis Müller on 15.05.24.
//
#include "QSynthi2/AudioProcessing/AJAudioProcessor.h"
#include "QSynthi2/Data.h"

extern Data sharedData;


AJAudioProcessor::AJAudioProcessor() {
    constexpr int SIM_SIZE = 128;
    sim = std::dynamic_pointer_cast<Simulation>(std::make_shared<QuantumSimulation>(QuantumSimulation(SIM_SIZE,SIM_SIZE)
        .barrierPotential({-0.0, NAN}, 2, {{-0.2, -0.1}, {0.1, 0.2}}, 1e30)
        .parabolaPotential({0, 0}, {2, 1.5})
        .gaussianDistribution({-0.4, 0}, {0.25, 0.25}, {4, 0})));
    sharedData.simWidth = SIM_SIZE;
    sharedData.simHeight = SIM_SIZE;

    synth.setVoiceStealingEnabled (false); // TODO: Parameter
    for (auto i = 0; i < 15; ++i)
        synth.addVoice (new Voice(sim));
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
    sharedData.hashMapStopwatch.print();
    sharedData.functionCallStopwatch.print();
    sharedData.simulationStopwatch.print();
}
