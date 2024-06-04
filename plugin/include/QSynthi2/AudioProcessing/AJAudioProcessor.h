#pragma once

#include "QSynthi2/Juce.h"
#include <QSynthi2/Synthesizer/Synthesiser.h>
#include <QSynthi2/Simulation/Simulation.h>

class AJAudioProcessor {
public:

    AJAudioProcessor();
    ~AJAudioProcessor();

    void prepareToPlay(double newSampleRate, int newSamplesPerBlock);

    void processBlock(juce::AudioBuffer<float>& buffer, const juce::MidiBuffer& midiMessages);

protected:

    Decimal sampleRate;
    size_t samplesPerBlock;

    juce::MPEInstrument instrument { juce::MPEZone (juce::MPEZone::Type::lower, 15) };
    Synthesiser synth { instrument };



    // TODO: Remove
    size_t time = 0;
    size_t timestepCounter = 0;
    size_t bufferCounterDebug = 0;
    std::shared_ptr<Simulation> sim;
    ComplexMatrix* simFrameCurrent;
    long nextFrameRequest;
    SimThread* st;
};