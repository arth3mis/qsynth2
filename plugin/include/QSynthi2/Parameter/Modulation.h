#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include <QSynthi2/List/list.h>

class FloatParameter;

class Modulation {
public:

    Modulation(juce::String modulationSource, std::shared_ptr<FloatParameter> &amount);

    float getNormalizedBaseValue(const std::unordered_map<juce::String, float>& modulationData);

    float getNormalized(const std::unordered_map<juce::String, float>& modulationData);

protected:

    juce::String modulationSource;
    std::shared_ptr<FloatParameter> amount;

    List<Modulation> modulations;

};