#include "QSynthi2/AudioProcessing/AJAudioProcessor.h"
#include "QSynthi2/Simulation/QuantumSimulation.h"
#include "QSynthi2/Synthesizer/Voice.h"
#include "QSynthi2/Data.h"

extern Data sharedData;


AJAudioProcessor::AJAudioProcessor() {
    constexpr int SIM_SIZE = 128;
    auto simulation = std::dynamic_pointer_cast<Simulation>(std::make_shared<QuantumSimulation>(QuantumSimulation(SIM_SIZE,SIM_SIZE)
        .barrierPotential({-0.0, NAN}, 2, {{-0.2, -0.1}, {0.1, 0.2}}, 1e30)
        .parabolaPotential({0, 0}, {2, 1.5})
        .gaussianDistribution({-0.4, 0}, {0.25, 0.25}, {4, 0})));
    sharedData.simWidth = SIM_SIZE;
    sharedData.simHeight = SIM_SIZE;

    simulationThread = new SimulationThread(simulation);
    simulationThread->started = true;
    sharedData.totalStopwatch.start();

    // sharedData.setSimulationDisplayFrame(std::dynamic_pointer_cast<QuantumSimulation>(sim)->getPsi());

    synth.setVoiceStealingEnabled (false); // TODO: Parameter
    for (auto i = 0; i < 15; ++i)
        synth.addVoice (new Voice());
}

AJAudioProcessor::~AJAudioProcessor() {
    simulationThread->terminate = true;
    sharedData.totalStopwatch.stop();
    juce::Logger::writeToLog("Avg. FPS = " + juce::String(simulationThread->newestFrame / (sharedData.totalStopwatch.get() / 1000000000.0), 1));
    delete simulationThread;
}

void AJAudioProcessor::prepareToPlay(Decimal newSampleRate, int newSamplesPerBlock) {
    sampleRate = newSampleRate;
    samplesPerBlock = static_cast<size_t>(newSamplesPerBlock);
    synth.prepareToPlay(newSampleRate, newSamplesPerBlock);

    sharedData.frameBufferTimestamps = Eigen::ArrayX<Decimal>(samplesPerBlock);

    juce::ignoreUnused (samplesPerBlock);

}

void AJAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, const juce::MidiBuffer &midiMessages) {

    // Process MIDI
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    // TODO: Reset simulation when no note is played?

    // TODO: Get from Parameters
    Eigen::ArrayX<Decimal> simulationFrameIncrement = Eigen::ArrayX<Decimal>(buffer.getNumSamples()).setOnes() * 100 / sampleRate;

    auto frameBufferNewFirstFrame = static_cast<size_t>(floor(currentSimulationFrame));

    juce::Logger::writeToLog("new: " + juce::String(sharedData.frameBufferFirstFrame) + " old: " + juce::String(frameBufferNewFirstFrame));

    sharedData.frameBuffer.remove(0, frameBufferNewFirstFrame - sharedData.frameBufferFirstFrame);
    sharedData.frameBufferFirstFrame = frameBufferNewFirstFrame;


    // Calculate needed timestamps
    for (long sample = 0; sample < static_cast<long>(samplesPerBlock); sample++) {
        sharedData.frameBufferTimestamps[sample] = currentSimulationFrame - static_cast<Decimal>(frameBufferNewFirstFrame);
        currentSimulationFrame += simulationFrameIncrement[sample];
    }

    size_t neededSimulationFrames = static_cast<size_t>(ceil(currentSimulationFrame)) - sharedData.frameBufferFirstFrame - sharedData.frameBuffer.size();




    //  - retrieve modData from synth
    List<ModulationData> modulationData;

    //  - update parameters in simulation: bufferSize, dt (later: gaussian, potential etc)
    // todo buffer must be at least 2x requested frame count (?)
    simulationThread->updateParameters(sharedData.parameters, modulationData);


    // TODO
    //  - (later) ask simulation how many frames are ready.
    //      - If too few:
    //          - If offline rendering: Wait until simulation is ready
    //          - Else: slow down simulation speed for audio processing to just use available frames
    while (simulationThread->frameReadyCount() <= neededSimulationFrames) {
        // busy wait
    }

    //  - request n frames from simulation (pass n, receive shared pointers; sim updates atomic currentBufferN)
    //  - save timesteps for scanner (vector<shared_ptr<Frame>>, prob. in Data.h)
    auto newFrames = simulationThread->getFrames(neededSimulationFrames);
    sharedData.frameBuffer.insert(sharedData.frameBuffer.end(), newFrames.begin(), newFrames.end());
    juce::Logger::writeToLog("got frames: " + juce::String(newFrames.size())
        + " of " + juce::String(neededSimulationFrames));


    // Process Audio
    auto samples = synth.generateNextBlock();

    for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            buffer.addSample(channel, i, static_cast<float>(samples[i]));
        }
    }
}
