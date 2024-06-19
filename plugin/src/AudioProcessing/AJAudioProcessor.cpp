#include "QSynthi2/AudioProcessing/AJAudioProcessor.h"
#include "QSynthi2/Simulation/QuantumSimulation.h"
#include "QSynthi2/Simulation/VideoSimulation.h"
#include "QSynthi2/Synthesizer/Voice.h"
#include "QSynthi2/Data.h"

extern Data sharedData;

#define SIMULATION_SIZE 128

AJAudioProcessor::AJAudioProcessor() {
    auto simulation = std::dynamic_pointer_cast<Simulation>(std::make_shared<QuantumSimulation>(QuantumSimulation(SIMULATION_SIZE,SIMULATION_SIZE)
        .barrierPotential({-0.0, NAN}, 2, {{-0.2, -0.1}, {0.1, 0.2}}, 1e30)
        .parabolaPotential({0, 0}, {2, 1.5})
        .gaussianDistribution({-0.4, 0}, {0.25, 0.25}, {4, 0})));
    sharedData.simulationWidth = SIMULATION_SIZE;
    sharedData.simulationHeight = SIMULATION_SIZE;

    simulationThread = new SimulationThread(simulation);

    synth.setVoiceStealingEnabled (false); // TODO: Parameter
    for (auto i = 0; i < 15; ++i)
        synth.addVoice (new Voice(std::make_shared<VoiceData>()));
}

AJAudioProcessor::~AJAudioProcessor() {
    simulationThread->terminate = true;
    delete simulationThread;
}

void AJAudioProcessor::prepareToPlay(const Decimal newSampleRate, const int newSamplesPerBlock) {
    sampleRate = newSampleRate;
    samplesPerBlock = static_cast<size_t>(newSamplesPerBlock);
    sharedData.parameters->prepareToPlay(newSampleRate, newSamplesPerBlock);
    synth.prepareToPlay(newSampleRate, newSamplesPerBlock);

    sharedData.frameBufferTimestamps = Eigen::ArrayX<Decimal>(samplesPerBlock);

    juce::ignoreUnused (samplesPerBlock);

}

void AJAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, const juce::MidiBuffer &midiMessages) {

    // Process MIDI
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // buttons
    if (sharedData.resetSimulation) {
        if (sharedData.newSimulation.isNotEmpty()) {
            // create new simulation
            const auto simulation = std::dynamic_pointer_cast<Simulation>(std::make_shared<VideoSimulation>(
                    VideoSimulation(SIMULATION_SIZE, SIMULATION_SIZE, sharedData.newSimulation)));
            sharedData.simulationWidth = SIMULATION_SIZE;
            sharedData.simulationHeight = SIMULATION_SIZE;
            sharedData.newSimulation = "";
            simulationThread->resetSimulation(simulation);
        } else {
            simulationThread->resetSimulation();
        }
        if (!simulationThread->isSimulationContinuous()) {
            sharedData.resetFrameBuffer();
            sharedData.setSimulationDisplayFrame(simulationThread->getStartFrame());
        }
        sharedData.resetSimulation = false;
    }

    // Update simulation parameters
    auto activeVoices = synth.getActiveVoices();
    List<ModulationData*> modulationDataList = activeVoices.map<ModulationData*>([](Voice* v){ return v->getModulationData(); });
    Eigen::ArrayX<Decimal> simulationStepsPerSecond = sharedData.parameters->simulationStepsPerSecond->getModulated(modulationDataList);
    if (sharedData.videoFps > 0) {
        simulationStepsPerSecond = Eigen::ArrayX<Decimal>::Ones(simulationStepsPerSecond.size()) * sharedData.videoFps;
    }
    Eigen::ArrayX<Decimal> simulationFrameIncrement = simulationStepsPerSecond / sampleRate;
    simulationThread->updateParameters(sharedData.parameters, modulationDataList);

    // todo maybe exclude release state voices
    if (activeVoices.empty() && !simulationThread->isSimulationContinuous()) {
        simulationThread->resetSimulation();
        sharedData.resetFrameBuffer();
        sharedData.setSimulationDisplayFrame(simulationThread->getStartFrame());
        simulationThread->started = false;
    } else {
        simulationThread->started = true;

        const auto frameBufferNewFirstFrame = static_cast<size_t>(floor(currentSimulationFrame));

        // Remove past frames
        size_t framesToRemove = std::min(frameBufferNewFirstFrame - sharedData.frameBufferFirstFrame, sharedData.frameBuffer.size());
        jassert(sharedData.frameBuffer.size() >= framesToRemove);
        sharedData.frameBuffer.remove(0, framesToRemove);
        sharedData.frameBufferFirstFrame += framesToRemove;

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
