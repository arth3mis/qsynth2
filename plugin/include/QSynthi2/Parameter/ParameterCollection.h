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
        0.2f);



    ModulatedParameterFloat* lineOfInterestX = add<ModulatedParameterFloat>(
            "Line of interest x",
            juce::NormalisableRange<float>(-1, 1, 0, 1.f, false),
            0.f);
    ModulatedParameterFloat* lineOfInterestY = add<ModulatedParameterFloat>(
            "Line of interest y",
            juce::NormalisableRange<float>(-1, 1, 0, 1.f, false),
            0.f);
    ModulatedParameterFloat* lineOfInterestRotation = add<ModulatedParameterFloat>(
            "Line of interest rotation",
            juce::NormalisableRange<float>(-1, 1, 0, 1.f, false),
            0.f);



    // Add modulation slots
    ParameterCollection() {
        addModulationSlots(8);
    }
};