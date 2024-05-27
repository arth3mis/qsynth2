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
        juce::NormalisableRange<float>(0, 1, 0, 0.5, false),
        1);
    ModulatedParameterFloat* simulationStepsPerSecond = add<ModulatedParameterFloat>(
        "Simulation steps per second",
        juce::NormalisableRange<float>(10, 400, 1, 0.5, false),
        100);


    // Add modulation slots
    ParameterCollection() {
        addModulationSlots(8);
    }
};