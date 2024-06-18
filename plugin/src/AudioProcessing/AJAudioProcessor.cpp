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
    sharedData.simulationWidth = SIM_SIZE;
    sharedData.simulationHeight = SIM_SIZE;

    simulationThread = new SimulationThread(simulation);

    // sharedData.setSimulationDisplayFrame(std::dynamic_pointer_cast<QuantumSimulation>(sim)->getPsi());

    synth.setVoiceStealingEnabled (false); // TODO: Parameter
    for (auto i = 0; i < 15; ++i)
        synth.addVoice (new Voice(std::make_shared<VoiceData>()));
}

AJAudioProcessor::~AJAudioProcessor() {
    simulationThread->terminate = true;
    // if (simulationThread->newestFrame > 0)
    //     juce::Logger::writeToLog("simulation thread sleep times: " + juce::String(simulationThread->sleepCounter)
    //         + " / " + juce::String(simulationThread->newestFrame+1) + " frames = "
    //         + juce::String(static_cast<double>(simulationThread->sleepCounter)/(simulationThread->newestFrame+1)));
    delete simulationThread;
}

void AJAudioProcessor::prepareToPlay(Decimal newSampleRate, int newSamplesPerBlock) {
    sampleRate = newSampleRate;
    samplesPerBlock = static_cast<size_t>(newSamplesPerBlock);
    synth.prepareToPlay(newSampleRate, newSamplesPerBlock);

    sharedData.frameBufferTimestamps = Eigen::ArrayX<Decimal>(samplesPerBlock);

    totalStopwatch.start();

    juce::ignoreUnused (samplesPerBlock);

}

void AJAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, const juce::MidiBuffer &midiMessages) {

    // Process MIDI
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // buttons
    if (sharedData.resetSimulation) {
        simulationThread->resetSimulation();
        sharedData.resetSimulation = false;
    }

    // Update simulation parameters
    auto activeVoices = synth.getActiveVoices();
    List<ModulationData*> modulationDataList = activeVoices.map<ModulationData*>([](Voice* v){ return v->getModulationData(); });
    Eigen::ArrayX<Decimal> simulationFrameIncrement = sharedData.parameters->simulationStepsPerSecond->getModulated(modulationDataList) / sampleRate;
    simulationThread->updateParameters(sharedData.parameters, modulationDataList);

    // todo maybe exclude release state voices
    if (activeVoices.empty()) {
        simulationThread->resetSimulation();
        sharedData.setSimulationDisplayFrame(simulationThread->getStartFrame());
    } else {
        const auto frameBufferNewFirstFrame = static_cast<size_t>(floor(currentSimulationFrame));

        // Remove past frames
        sharedData.frameBuffer.remove(0, std::min(frameBufferNewFirstFrame - sharedData.frameBufferFirstFrame, sharedData.frameBuffer.size()));
        sharedData.frameBufferFirstFrame = frameBufferNewFirstFrame;

        // Calculate timestamps
        for (long sample = 0; sample < static_cast<long>(samplesPerBlock); sample++) {
            sharedData.frameBufferTimestamps[sample] = currentSimulationFrame - static_cast<Decimal>(frameBufferNewFirstFrame);
            currentSimulationFrame += simulationFrameIncrement[sample];
        }

        size_t neededSimulationFrames = static_cast<size_t>(ceil(currentSimulationFrame)) + 1 - sharedData.frameBufferFirstFrame - sharedData.frameBuffer.size();

        // TODO
        //  - If offline rendering: Busy wait until simulation is ready
        //  - Else: slow down simulation speed for audio processing to just use available frames
        if (simulationThread->frameReadyCount() <= neededSimulationFrames) {
            juce::Logger::writeToLog("Busy wait for simulation thread.");
            while (simulationThread->frameReadyCount() <= neededSimulationFrames) {
                // busy wait
            }
        }
        // juce::Logger::writeToLog("Simulation thread is " + juce::String(simulationThread->frameReadyCount() - neededSimulationFrames) + " frames ahead.");

        // append the shared frame buffer
        // this also sets the latest simulation frame as the display frame (if a new one arrived)
        const auto newFrames = simulationThread->getFrames(neededSimulationFrames);
        sharedData.appendFrameBuffer(newFrames);

        // juce::Logger::writeToLog("got frames: " + juce::String(newFrames.size()) + " of " + juce::String(neededSimulationFrames));


        // Process Audio
        auto samples = synth.generateNextBlock();

        for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
            for (int i = 0; i < buffer.getNumSamples(); i++) {
                buffer.addSample(channel, i, static_cast<float>(samples[i]));
            }
        }
    }
}
