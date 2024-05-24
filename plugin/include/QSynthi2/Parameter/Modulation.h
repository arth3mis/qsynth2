#pragma once

#include "QSynthi2/Juce.h"
#include <QSynthi2/types.h>

class ModulatedParameterFloat;

class Modulation {
public:

    class Sources {
    public:
        inline static const juce::String VELOCITY{"velocity"};
        inline static const juce::String PITCH{"pitch"};
        inline static const juce::String Y{"y"};
        inline static const juce::String Z{"z"};

        inline static List<juce::String> ALL{VELOCITY, PITCH, Y, Z};
    };


    Modulation(juce::String modulationSource, ModulatedParameterFloat* amount);

    // TODO: const modulation Data
    const Eigen::ArrayX<Decimal> getModulatedNormalized(ModulationData& modulationData);


protected:

    juce::String modulationSource;
    ModulatedParameterFloat* amount;

};