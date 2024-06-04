#include "QSynthi2/AudioProcessing/AJAudioProcessor.h"
#include "QSynthi2/Simulation/QuantumSimulation.h"
#include "QSynthi2/Synthesizer/Voice.h"
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

    st = new SimThread(sim);
    st->started = true;
    sharedData.totalStopwatch.start();
    nextFrameRequest = 0;

    // sharedData.setSimulationDisplayFrame(std::dynamic_pointer_cast<QuantumSimulation>(sim)->getPsi());

    synth.setVoiceStealingEnabled (false); // TODO: Parameter
    for (auto i = 0; i < 15; ++i)
        synth.addVoice (new Voice());
}

AJAudioProcessor::~AJAudioProcessor() {
    st->terminate = true;
    sharedData.totalStopwatch.stop();
    sharedData.totalStopwatch.print();
    juce::Logger::writeToLog("Avg. FPS = " + juce::String(st->newestFrame / (sharedData.totalStopwatch.get()/1000000000.0), 1));
    delete st;
}

void AJAudioProcessor::prepareToPlay(Decimal newSampleRate, int newSamplesPerBlock) {
    sampleRate = newSampleRate;
    samplesPerBlock = static_cast<size_t>(newSamplesPerBlock);
    synth.prepareToPlay(newSampleRate, newSamplesPerBlock);

    juce::ignoreUnused (samplesPerBlock);

}

void AJAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, const juce::MidiBuffer &midiMessages) {
    sharedData.blockStopwatch.start();
    /*
     *     for (const auto &m : midiMessages) {
    const auto midiEvent = m.getMessage();
    const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());

    juce::Logger::writeToLog(midiEvent.getDescription());
    }
     */


    sharedData.functionCallStopwatch.start();
    // Process MIDI
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());



    // TODO: Get from Parameters
    Eigen::ArrayX<Decimal> simulationFrameIncrement = Eigen::ArrayX<Decimal>(buffer.getNumSamples()).setOnes() * 100 / sampleRate;

    auto firstSimulationFrameIndex = static_cast<size_t>(floor(sharedData.currentSimulationFrameIndex));

    // Calculate needed timestamps
    for (long sample = 0; sample < static_cast<long>(samplesPerBlock); sample++) {
        sharedData.currentSimulationFrameIndex += simulationFrameIncrement[sample];
        sharedData.relativeSimulationFrameIndices[sample] = sharedData.currentSimulationFrameIndex - static_cast<Decimal>(firstSimulationFrameIndex);
    }

    size_t neededSimulationFrames = static_cast<size_t>(ceil(sharedData.currentSimulationFrameIndex)) - firstSimulationFrameIndex;



    // TODO Process Simulation
    //  - retrieve modData from synth
    //  - update parameters in simulation: bufferSize, dt (later: gaussian, potential etc)
    //  for each sample: ✅
    //      - calculate timesteps per sample (can change inside the block) ✅
    //      - save 1D array of frame interpolation indices ✅
    //  - (later) ask simulation how many frames are ready.
    //      - If too few:
    //          - If offline rendering: Wait until simulation is ready
    //          - Else: slow down simulation speed for audio processing to just use available frames
    //  - request n frames from simulation (pass n, receive shared pointers; sim updates atomic currentBufferN)
    //  - save timesteps for scanner (vector<shared_ptr<Frame>>, prob. in Data.h)
    //

    // Process Audio
    auto samples = synth.generateNextBlock();
    for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            buffer.addSample(channel, i, static_cast<float>(samples[i]));
        }
    }

    sharedData.functionCallStopwatch.stop();

    bufferCounterDebug += buffer.getNumSamples();

    // TEMP simulation
    constexpr int steps = 100;
    if (time++ % std::max((size_t)1, static_cast<size_t>(samplerate / buffer.getNumSamples() / steps)) != 0) {
        return;
    }
    timestepCounter++;

    simFrameCurrent = st->getFrame(nextFrameRequest);
    if (simFrameCurrent == nullptr) {
        if (timestepCounter % steps == 0)
            juce::Logger::writeToLog("frame request failed for i="+juce::String(nextFrameRequest));
    } else {
        nextFrameRequest = st->newestFrame;
        sharedData.setSimulationDisplayFrame(*simFrameCurrent);
        sharedData.setSimulationScanFrame(*simFrameCurrent);
        if (timestepCounter % steps == 0)
            juce::Logger::writeToLog(juce::String(st->newestFrame) + " frames created, " + juce::String(timestepCounter)
                +" requested. total size [GB] = ~"+juce::String(static_cast<double>(st->newestFrame) * 128*128*sizeof(Complex) / 1000000000, 3));
    }
    // simFrameCurrent = sim->getNextFrame(0.2, {});
    // sharedData.setSimulationDisplayFrame(simFrameCurrent);//.map<num>([](const cnum c){ return std::abs(c); }));

    sharedData.blockStopwatch.stop();

    if (timestepCounter % steps != 0)
        return;

    juce::Logger::writeToLog("samples = "+juce::String(bufferCounterDebug));//+"; timesteps = "+juce::String(timestepCounter));
    juce::Logger::writeToLog("Avg. FPS = " + juce::String(st->newestFrame / (sharedData.totalStopwatch.get()/1000000000.0), 1));

    long per = bufferCounterDebug / samplerate;
    sharedData.blockStopwatch.print(per, "second");
    sharedData.parameterStopwatch.print(per, "second");
    sharedData.modulationStopwatch.print(per, "second");
    sharedData.hashMapStopwatch.print(per, "second");
    sharedData.functionCallStopwatch.print(per, "second");

    // per = st->newestFrame;
    // sharedData.simPotStopwatch.print(per, "frame");
    // sharedData.simFftStopwatch.print(per, "frame");
    // sharedData.simKinStopwatch.print(per, "frame");
}
