#pragma once

#include "juce_audio_processors/juce_audio_processors.h"

class Modulation;

class Modulatable {

public:

    virtual ~Modulatable() = 0;

    virtual float getNormalizedBaseValue(const std::unordered_map<juce::String, float>& modulationData) = 0;

    virtual float getNormalized(const std::unordered_map<juce::String, float>& modulationData);

protected:

    std::vector<Modulation*> modulations;

};
