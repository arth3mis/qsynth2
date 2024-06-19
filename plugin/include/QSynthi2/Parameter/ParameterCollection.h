#pragma once


#include "Parameters.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"
#include "QSynthi2/Parameter/ExponentialRange.h"

class ParameterCollection : public Parameters {
public:

    // Declare Parameters here
    ModulatedParameterFloat* simulationSpeedFactor = add<ModulatedParameterFloat>(
            "Simulation speed",
            juce::NormalisableRange<float>(0, 400, 0, 0.7f, false),
            10);
    ModulatedParameterFloat* simulationStepsPerSecond = add<ModulatedParameterFloat>(
            "Simulation steps per second",
            juce::NormalisableRange<float>(10, 1000, 1, 0.5f, false),
            100);
    ModulatedParameterFloat* simulationBufferSeconds = add<ModulatedParameterFloat>(
            "Simulation buffer seconds",
            juce::NormalisableRange<float>(0, 2, 0, 0.5f, false),
            0.2f);

    juce::AudioParameterChoice* videoToDecimalMethod = add<juce::AudioParameterChoice>(juce::ParameterID(
            "Simulation mapping value", VERSION), "Simulation mapping value",
            juce::StringArray{"Luminance", "Red", "Green", "Blue"}, 0);

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
    ModulatedParameterFloat* audificationSmoothing = add<ModulatedParameterFloat>(
            "Audification scanline-smoothing",
            juce::NormalisableRange<float>(0, 1, 0, 0.5f, false),
            0.2f);
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


    ModulatedParameterFloat *gaussianOffsetX = add<ModulatedParameterFloat>(
            "Gaussian: x offset",
            juce::NormalisableRange<float>(-1, 1, 0, 1.f, false),
            -0.45f);
    ModulatedParameterFloat *gaussianOffsetY = add<ModulatedParameterFloat>(
            "Gaussian: y offset",
            juce::NormalisableRange<float>(-1, 1, 0, 1.f, false),
            0.f);
    ModulatedParameterFloat *gaussianStretchX = add<ModulatedParameterFloat>(
            "Gaussian: x stretch",
            juce::NormalisableRange<float>(0, 1, 0, 1.f, false),
            0.25f);
    ModulatedParameterFloat *gaussianStretchY = add<ModulatedParameterFloat>(
            "Gaussian: y stretch",
            juce::NormalisableRange<float>(0, 1, 0, 1.f, false),
            0.25f);
    ModulatedParameterFloat *gaussianImpulseX = add<ModulatedParameterFloat>(
            "Gaussian: x impulse",
            juce::NormalisableRange<float>(-10, 10, 0, 1.f, false),
            4.f);
    ModulatedParameterFloat *gaussianImpulseY = add<ModulatedParameterFloat>(
            "Gaussian: y impulse",
            juce::NormalisableRange<float>(-10, 10, 0, 1.f, false),
            0.f);
    ModulatedParameterFloat *linearAngle = add<ModulatedParameterFloat>(
            "Linear potential: rotation",
            juce::NormalisableRange<float>(-360, 360, 0, 1.f, false),
            0.f);
    ModulatedParameterFloat *linearFactor = add<ModulatedParameterFloat>(
            "Linear potential: strength",
            juce::NormalisableRange<float>(-20, 20, 0, 1.f, false),
            0.f);
    ModulatedParameterFloat *parabolaOffsetX = add<ModulatedParameterFloat>(
            "Parabola potential: x offset",
            juce::NormalisableRange<float>(-1, 1, 0, 1.f, false),
            0.f);
    ModulatedParameterFloat *parabolaOffsetY = add<ModulatedParameterFloat>(
            "Parabola potential: y offset",
            juce::NormalisableRange<float>(-1, 1, 0, 1.f, false),
            0.f);
    ModulatedParameterFloat *parabolaFactorX = add<ModulatedParameterFloat>(
            "Parabola potential: x strength",
            juce::NormalisableRange<float>(0, 20, 0, 1.f, false),
            2.f);
    ModulatedParameterFloat *parabolaFactorY = add<ModulatedParameterFloat>(
            "Parabola potential: y strength",
            juce::NormalisableRange<float>(0, 20, 0, 1.f, false),
            1.5f);
    ModulatedParameterFloat *barrierOffsetX = add<ModulatedParameterFloat>(
            "Barrier: x offset",
            juce::NormalisableRange<float>(-1.01f, 1, 0, 1.f, false),
            0.f);
    ModulatedParameterFloat *barrierWidth = add<ModulatedParameterFloat>(
            "Barrier: width",
            juce::NormalisableRange<float>(0, 6, 0, 1.f, false),
            2.f);
    ModulatedParameterFloat *barrierSlit1Start = add<ModulatedParameterFloat>(
            "Barrier slit 1: start",
            juce::NormalisableRange<float>(-1.01f, 1, 0, 1.f, false),
            -0.2f);
    ModulatedParameterFloat *barrierSlit1End = add<ModulatedParameterFloat>(
            "Barrier slit 1: end",
            juce::NormalisableRange<float>(-1.01f, 1, 0, 1.f, false),
            -0.1f);
    ModulatedParameterFloat *barrierSlit2Start = add<ModulatedParameterFloat>(
            "Barrier slit 2: start",
            juce::NormalisableRange<float>(-1.01f, 1, 0, 1.f, false),
            0.1f);
    ModulatedParameterFloat *barrierSlit2End = add<ModulatedParameterFloat>(
            "Barrier slit 2: end",
            juce::NormalisableRange<float>(-1.01f, 1, 0, 1.f, false),
            0.2f);

    ModulatedParameterFloat* baseFrequency = add<ModulatedParameterFloat>(
            "Frequency",
            ExponentialRange(10, 2560),
            110);

    // Add modulation slots
    ParameterCollection() {
        addModulationSlots(12);
    }
};