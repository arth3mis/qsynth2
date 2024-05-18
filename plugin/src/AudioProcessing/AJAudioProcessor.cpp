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

    sharedData.setSimulationDisplayFrame(*std::dynamic_pointer_cast<QuantumSimulation>(sim)->getPsi());

    synth.setVoiceStealingEnabled (false); // TODO: Parameter
    for (auto i = 0; i < 15; ++i)
        synth.addVoice (new Voice(sim));
}

void AJAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    synth.setCurrentPlaybackSampleRate(sampleRate);

    juce::ignoreUnused (samplesPerBlock);

}

void AJAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {

    /*
     *     for (const auto &m : midiMessages) {
    const auto midiEvent = m.getMessage();
    const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());

    juce::Logger::writeToLog(midiEvent.getDescription());
    }
     */


    sharedData.functionCallStopwatch.start();
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    sharedData.functionCallStopwatch.stop();


    bufferCounterDebug += buffer.getNumSamples();

    // TEMP simulation
    const int steps = 100;
    if (time++ % static_cast<size_t>(44100 / buffer.getNumSamples() / steps) != 0) {
        return;
    }
    timestepCounter++;

    sharedData.simulationStopwatch.start();
    simFrameCurrent = sim->getNextFrame(0.2, {});
    sharedData.setSimulationDisplayFrame(*simFrameCurrent);//.map<num>([](const cnum c){ return std::abs(c); }));
    sharedData.simulationStopwatch.stop();

    if (timestepCounter % steps != 0)
        return;

    juce::Logger::writeToLog("samples = "+juce::String(bufferCounterDebug)+"; timesteps = "+juce::String(timestepCounter));

    long per = bufferCounterDebug;
    sharedData.parameterStopwatch.print(per, "sample");
    sharedData.modulationStopwatch.print(per, "sample");
    sharedData.functionCallStopwatch.print(per, "sample");

    per = timestepCounter;
    sharedData.simulationStopwatch.print(per, "timestep");

    sharedData.simPotStopwatch.print(per, "timestep");
    sharedData.simFftStopwatch.print(per, "timestep");
    sharedData.simKinStopwatch.print(per, "timestep");
}
