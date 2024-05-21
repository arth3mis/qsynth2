#pragma once

#include <QSynthi2/Synthesizer/Voice.h>
#include <QSynthi2/Synthesizer/Synthesiser.h>

#include "QSynthi2/Juce.h"

class AJAudioProcessor {
public:

    AJAudioProcessor();
    ~AJAudioProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock);

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

protected:

    juce::MPEInstrument instrument { juce::MPEZone (juce::MPEZone::Type::lower, 15) };
    Synthesiser synth { instrument };

    size_t time = 0;
    size_t timestepCounter = 0;
    size_t bufferCounterDebug = 0;
    std::shared_ptr<Simulation> sim;
    CSimMatrix* simFrameCurrent;
    long nextFrameRequest;
    SimThread* st;

};