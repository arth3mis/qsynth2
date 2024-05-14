#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "ParameterHolder.h"
#include <QSynthi2/List/list.h>

class FloatParameter;

class Modulation {
public:

    Modulation(juce::String modulationSource, std::shared_ptr<FloatParameter> &amount);

    float getNormalizedBaseValue(const ModulationData& modulationData);

    float getNormalized(const ModulationData& modulationData);

protected:

    juce::String modulationSource;
    std::shared_ptr<FloatParameter> amount;

    List<Modulation> modulations;

};