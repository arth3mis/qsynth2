#pragma once

#include <QSynthi2/Synthesizer/Voice.h>
#include <QSynthi2/Synthesizer/Synthesiser.h>

#include "juce_audio_processors/juce_audio_processors.h"

class AJAudioProcessor {
public:

    AJAudioProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock);

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

protected:

    juce::MPEInstrument instrument { juce::MPEZone (juce::MPEZone::Type::lower, 15) };
    Synthesiser synth { instrument };

    size_t time = 0;
    size_t timestepCounter = 0;
    size_t bufferCounterDebug = 0;
    std::shared_ptr<Simulation> sim;
    const CSimMatrix *simFrameCurrent;

};