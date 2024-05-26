#pragma once

#include "QSynthi2/Juce.h"
#include <QSynthi2/types.h>

class ModulatedParameterFloat;

class Modulation {
public:

    class Sources {
    public:
        inline static const juce::String VELOCITY{"Velocity"};
        inline static const juce::String PITCH{"Pitch"};
        inline static const juce::String Y{"Keyboard Y"};
        inline static const juce::String Z{"Keyboard Z"};

        inline static List<juce::String> ALL{VELOCITY, PITCH, Y, Z};
    };

    Modulation();

    Modulation(juce::String modulationSource, ModulatedParameterFloat* amount);

    Eigen::ArrayX<Decimal> getModulatedNormalized(const ModulationData& modulationData);

    void setModulatedParameterId(juce::String newModulatedParameterId);
    juce::String getModulatedParameterId();

    void setModulationSource(juce::String newModulationSource);

    void setAmountParameter(ModulatedParameterFloat* newAmount);

    void prepareToPlay(int newSamplesPerBlock);

protected:

    int samplesPerBlock;

    juce::String modulatedParameterId;
    juce::String modulationSource;
    ModulatedParameterFloat* amount = nullptr;

};