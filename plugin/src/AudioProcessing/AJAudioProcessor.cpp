#include "QSynthi2/AudioProcessing/AJAudioProcessor.h"
#include "QSynthi2/Simulation/QuantumSimulation.h"
#include "QSynthi2/Synthesizer/Voice.h"
#include "QSynthi2/Data.h"

extern Data sharedData;


AJAudioProcessor::AJAudioProcessor() {
    constexpr int SIM_SIZE = 128;
    auto simulation = std::dynamic_pointer_cast<Simulation>(std::make_shared<QuantumSimulation>(QuantumSimulation(SIM_SIZE,SIM_SIZE)
        .barrierPotential(BARRIER_VERTICAL, 0, 2, {{-0.2, -0.1}, {0.1, 0.2}}, 1e30)
        .parabolaPotential({0, 0}, {2, 1.5})
        .gaussianDistribution({-0.4, 0}, {0.25, 0.25}, {4, 0})));
    sharedData.simulationWidth = SIM_SIZE;
    sharedData.simulationHeight = SIM_SIZE;
    sharedData.barrierOffset = 0;
    sharedData.barrierWidth = 2;
    sharedData.barrierSlits = {{-0.2, -0.1}, {0.1, 0.2}};

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

    juce::Logger::writeToLog(isOfflineRendering ? "offline rendering detected" : "live/realtime mode");
}

void AJAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, const juce::MidiBuffer &midiMessages) {

    // Process MIDI
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // buttons
    if (sharedData.resetSimulation) {
        simulationThread->resetSimulation();
        if (!simulationThread->isSimulationContinuous()) {
            sharedData.resetFrameBuffer();
            sharedData.setSimulationDisplayFrame(simulationThread->getStartFrame());
        }
        sharedData.resetSimulation = false;
    }

    // Update simulation parameters
    const auto activeVoices = synth.getActiveVoices();
    List<ModulationData*> modulationDataList = activeVoices.map<ModulationData*>([](Voice* v){ return v->getModulationData(); });
    Eigen::ArrayX<Decimal> simulationFrameIncrement = sharedData.parameters->simulationStepsPerSecond->getModulated(modulationDataList) / sampleRate;
    const bool simulationParametersChanged = simulationThread->updateParameters(sharedData.parameters, modulationDataList);

    // Update simulation buffer progress bar
    const int frameReadyCount = simulationThread->frameReadyCount();
    const Decimal simulationStepsPerSecond = sharedData.parameters->simulationStepsPerSecond->getSingleModulated(modulationDataList);
    const Decimal simulationBufferSeconds = sharedData.parameters->simulationBufferSeconds->getSingleModulated(modulationDataList);
    const size_t target = std::max(static_cast<size_t>(round(simulationBufferSeconds * simulationStepsPerSecond)), static_cast<size_t>(2));
    sharedData.simulationBufferProgressFraction = static_cast<Decimal>(frameReadyCount) / target;

    // always call the first time, then if nothing is playing (continuous: true for video, false for quantum)
    // todo maybe exclude release state voices
    if (firstRun || (activeVoices.empty() && !simulationThread->isSimulationContinuous())) {
        // reset simulation and buffer a) when running and playing stopped, b) on first run to reset after updating simulation parameters
        if (simulationRunning || firstRun) {
            simulationThread->resetSimulation();
            sharedData.resetFrameBuffer();
            sharedData.setSimulationDisplayFrame(simulationThread->getStartFrame());
            // simulationThread->started = false;  // 2025: simulatio always started to fill buffer
            simulationRunning = false;
            simulationThread->playing = false;
            firstRun = false;
        }
        // update display frame to enable live preview when not playing
        if (simulationParametersChanged) {
            sharedData.setSimulationDisplayFrame(simulationThread->getStartFrame());
        }
        // else/finally: do nothing, return to JUCE
    }
    // main audio logic, fetching simulation frames and generating the requested sample block
    else {
        // simulationThread->started = true;  // 2025: simulatio always started to fill buffer
        simulationRunning = true;
        simulationThread->playing = true;

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

        int neededSimulationFrames = static_cast<int>(ceil(currentSimulationFrame)) + 1 - sharedData.frameBufferFirstFrame - sharedData.frameBuffer.size();

        // TODO
        //  - If offline rendering: Busy wait until simulation is ready
        //  - Else: slow down simulation speed for audio processing to just use available frames
        //       2025 Else: repeat latest available frame until new ones are available
        if (isOfflineRendering && simulationThread->frameReadyCount() < neededSimulationFrames) {
            // juce::Logger::writeToLog("Busy wait for simulation thread.");
            int busyWaitCounter = 0;

            while (simulationThread->frameReadyCount() < neededSimulationFrames) {
                busyWaitCounter++;
                // ensure loop is not removed by compiler optimisation
                std::atomic_signal_fence(std::memory_order_seq_cst);
                // busy wait limit
                if (busyWaitCounter >= 10000000) {
                    juce::Logger::writeToLog("VERY LONG busy wait for simulation thread.");
                    break;
                }
            }
        }
        // juce::Logger::writeToLog("Simulation thread is " + juce::String(simulationThread->frameReadyCount() - neededSimulationFrames) + " frames ahead.");

        // move new frames from simulation buffer here
        auto newFrames = simulationThread->getFrames(neededSimulationFrames);
        // juce::Logger::writeToLog("got frames: " + juce::String(newFrames.size()) + " of " + juce::String(neededSimulationFrames));

        // got 0 new frames but at least 1 needed?
        if (newFrames.empty() && neededSimulationFrames > 0) {
            // push start frame
            if (sharedData.frameBuffer.empty())
                newFrames.push_back(simulationThread->getStartFrame());
            // push latest frame
            else
                newFrames.push_back(sharedData.frameBuffer.back());
        }

        // too few frames? repeat last frame until enough frames are ready
        while (newFrames.size() < neededSimulationFrames) {
            newFrames.push_back(newFrames.back());
        }

        // append shared frame buffer for audio processing
        // this also sets the latest simulation frame as the display frame (if a new one arrived)
        sharedData.appendFrameBuffer(newFrames);


        // Process Audio
        auto samples = synth.generateNextBlock();

        for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
            for (int i = 0; i < buffer.getNumSamples(); i++) {
                buffer.addSample(channel, i, static_cast<float>(samples[i]));
            }
        }
    }
}
