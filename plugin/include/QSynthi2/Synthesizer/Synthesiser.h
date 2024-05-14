#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "Voice.h"


class Synthesiser : public juce::MPESynthesiser {
public:

    explicit Synthesiser(juce::MPEInstrument& instrument) : juce::MPESynthesiser(instrument) {

    }

private:

};