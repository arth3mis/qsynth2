#pragma once

#include "QSynthi2/Juce.h"
#include <QSynthi2/types.h>

class ModulatedParameterFloat;

class Modulation {
public:

    class Sources {
    public:
        inline static const juce::String VELOCITY{"velocity"};
        inline static const juce::String PRESSURE{"pressure"};
        inline static const juce::String TIMBRE{"timbre"};

        inline static const List<juce::String> ALL{VELOCITY, PRESSURE, TIMBRE};
    };


    Modulation(juce::String modulationSource, ModulatedParameterFloat* amount);

    float getNormalizedBaseValue(const ModulationData& modulationData);

    float getNormalized(const ModulationData& modulationData);



protected:

    juce::String modulationSource;
    ModulatedParameterFloat* amount;

    List<Modulation> modulations;

};