#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_dsp/juce_dsp.h"

class Oscillator {

    virtual void getNextSamples(double phase, double frequency, size_t num,  fill) {

    }

};