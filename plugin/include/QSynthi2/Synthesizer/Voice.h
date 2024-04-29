#pragma once

#include <utility>

#include "juce_audio_processors/juce_audio_processors.h"
#include "Oscillator.h"

class Voice {
public:

    explicit Voice(std::shared_ptr<Oscillator> _oscillator) : oscillator(std::move(_oscillator)) {

    }

protected:

    std::shared_ptr<Oscillator> oscillator;


};