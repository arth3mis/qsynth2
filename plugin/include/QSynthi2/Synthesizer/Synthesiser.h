#pragma once

#include "QSynthi2/Juce.h"
#include "Voice.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"


class Synthesiser : public juce::MPESynthesiser {
public:

    explicit Synthesiser(juce::MPEInstrument& instrument_) : juce::MPESynthesiser(instrument_) {

    }

private:


};