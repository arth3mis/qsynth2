#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include <QSynthi2/List/list.h>
#include <QSynthi2/types.h>

class ModulatedParameterFloat;

class Modulation {
public:

    Modulation(juce::String modulationSource, std::shared_ptr<ModulatedParameterFloat> &amount);

    float getNormalizedBaseValue(const ModulationData& modulationData);

    float getNormalized(const ModulationData& modulationData);

protected:

    juce::String modulationSource;
    std::shared_ptr<ModulatedParameterFloat> amount;

    List<Modulation> modulations;

};