#pragma once

#include "QSynthi2/Juce.h"
#include <QSynthi2/Synthesizer/Synthesiser.h>
#include <QSynthi2/Simulation/Simulation.h>
#include <QSynthi2/Simulation/SimulationThread.h>

class AJAudioProcessor {
public:

    AJAudioProcessor();
    ~AJAudioProcessor();

    void prepareToPlay(double newSampleRate, int newSamplesPerBlock);

    void processBlock(juce::AudioBuffer<float>& buffer, const juce::MidiBuffer& midiMessages);

protected:

    Decimal sampleRate = 0;
    size_t samplesPerBlock = 0;

    juce::MPEInstrument instrument { juce::MPEZone (juce::MPEZone::Type::lower, 15) };
    Synthesiser synth { instrument };

    Decimal currentSimulationFrame = 0;

    SimulationThread* simulationThread;
};