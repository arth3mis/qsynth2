#pragma once

#include <QSynthi2/Synthesizer/Voice.h>
#include <QSynthi2/Synthesizer/Synthesiser.h>

#include "juce_audio_processors/juce_audio_processors.h"


class AJAudioProcessor {
public:

    AJAudioProcessor() {
        synth.setVoiceStealingEnabled (false); // TODO: Parameter
        for (auto i = 0; i < 15; ++i)
            synth.addVoice (new Voice());
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

};