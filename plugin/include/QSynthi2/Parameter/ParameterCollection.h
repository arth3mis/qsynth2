#pragma once


#include "Parameters.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"

class ParameterCollection : public Parameters {
public:

    // Declare Parameters here
    ModulatedParameterFloat* timbre = add<ModulatedParameterFloat>(
        "Timbre",
        juce::NormalisableRange<float>(0, 1),
        0);
    ModulatedParameterFloat* simulationSpeedFactor = add<ModulatedParameterFloat>(
        "Simulation speed",
        juce::NormalisableRange<float>(0, 100, 0, 0.7f, false),
        10);
    ModulatedParameterFloat* simulationStepsPerSecond = add<ModulatedParameterFloat>(
        "Simulation steps per second",
        juce::NormalisableRange<float>(10, 400, 1, 0.5f, false),
        100);
    ModulatedParameterFloat* simulationBufferSeconds = add<ModulatedParameterFloat>(
        "Simulation buffer seconds",
        juce::NormalisableRange<float>(0, 2, 0, 0.5f, false),
        0.1f);


    // Add modulation slots
    ParameterCollection() {
        addModulationSlots(8);
    }
};