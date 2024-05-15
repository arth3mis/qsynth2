#pragma once

#include <QSynthi2/Synthesizer/Voice.h>
#include <QSynthi2/Synthesizer/Synthesiser.h>

#include "juce_audio_processors/juce_audio_processors.h"


class AJAudioProcessor {
public:

    AJAudioProcessor() {
        constexpr int SIM_SIZE = 128;
        sim = std::make_shared<Simulation>(QuantumSimulation(SIM_SIZE,SIM_SIZE)
            .barrierPotential({-0.0, NAN}, 2, {{-0.2, -0.1}, {0.1, 0.2}}, 1e30)
            .parabolaPotential({0, 0}, {2, 1.5})
            .gaussianDistribution({-0.4, 0}, {0.25, 0.25}, {4, 0}));

        sharedData.simWidth = SIM_SIZE;
        sharedData.simHeight = SIM_SIZE;

        synth.setVoiceStealingEnabled (false); // TODO: Parameter
        for (auto i = 0; i < 15; ++i)
            synth.addVoice (new Voice(sim));
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) {
        synth.setCurrentPlaybackSampleRate(sampleRate);

        juce::ignoreUnused (samplesPerBlock);

    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
        synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    }

protected:

    juce::MPEInstrument instrument { juce::MPEZone (juce::MPEZone::Type::lower, 15) };
    Synthesiser synth { instrument };

    std::shared_ptr<Simulation> sim;

};