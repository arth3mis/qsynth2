#pragma once

#include "QSynthi2/types.h"

class Synthesiser : public juce::MPESynthesiser {
public:

    explicit Synthesiser(juce::MPEInstrument& instrument_) : juce::MPESynthesiser(instrument_) {
    }
};