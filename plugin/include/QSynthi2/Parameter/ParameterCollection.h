#pragma once


#include "Parameters.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"

class ParameterCollection : public Parameters {
public:

    // Declare Parameters here
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
    ModulatedParameterFloat* lineOfInterestLength = add<ModulatedParameterFloat>(
            "Line of interest length",
            juce::NormalisableRange<float>(0, 3, 0, 0.5f, false),
            1.f);
    ModulatedParameterFloat* lineOfInterestRotation = add<ModulatedParameterFloat>(
            "Line of interest rotation",
            juce::NormalisableRange<float>(-juce::MathConstants<float>::pi, juce::MathConstants<float>::pi, 0, 1.f, false),
            0.f);


    juce::AudioParameterChoice* sonificationMethod = add<juce::AudioParameterChoice>(
            juce::ParameterID("Sonification Method", Parameters::VERSION),
            "Sonification Method",
            juce::StringArray{"Audification", "Timbre mapping"},
            0);
    juce::AudioParameterChoice* timbreOvertoneLayout = add<juce::AudioParameterChoice>(
            juce::ParameterID("Overtone layout", Parameters::VERSION),
            "Overtone layout",
            juce::StringArray{"linear", "logarithmic"},
            1);
    ModulatedParameterFloat* timbreNumberOvertones = add<ModulatedParameterFloat>(
            "Number of overtones",
            juce::NormalisableRange<float>(0, 128, 1, 0.7f, false),
            32.f);


    ModulatedParameterFloat* volume = add<ModulatedParameterFloat>(
            "Volume",
            juce::NormalisableRange<float>(0, 1, 0, 1.3f, false),
            0.9f);


    ModulatedParameterFloat* envelope1Attack = add<ModulatedParameterFloat>(
            "Envelope 1 attack seconds",
            juce::NormalisableRange<float>(0.001f, 8, 0, .55f, false),
            0.050f);
    ModulatedParameterFloat* envelope1Decay = add<ModulatedParameterFloat>(
            "Envelope 1 decay seconds",
            juce::NormalisableRange<float>(0.001f, 8, 0, .5f, false),
            0.150f);
    ModulatedParameterFloat* envelope1Sustain = add<ModulatedParameterFloat>(
            "Envelope 1 sustain level",
            juce::NormalisableRange<float>(0, 1, 0, 1, false),
            1);
    ModulatedParameterFloat* envelope1Release = add<ModulatedParameterFloat>(
            "Envelope 1 release seconds",
            juce::NormalisableRange<float>(0.001f, 8, 0, .5f, false),
            0.150f);

    // Add modulation slots
    ParameterCollection() {
        addModulationSlots(8);
    }
};