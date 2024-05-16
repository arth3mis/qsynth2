//
// Created by Jannis Müller on 14.05.24.
//
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"
#include "QSynthi2/Parameter/Parameters.h"
#include "QSynthi2/Data.h"


extern Data sharedData;


ModulatedParameterFloat::ModulatedParameterFloat() :
        juce::AudioParameterFloat(juce::ParameterID("", Parameters::VERSION), "", juce::NormalisableRange<float>(), 0) {

}

ModulatedParameterFloat::ModulatedParameterFloat(const juce::String &name, const juce::NormalisableRange<float> &range,
                                                 float defaultValue, float sliderSmoothingSeconds) :
        juce::AudioParameterFloat(juce::ParameterID(name, Parameters::VERSION), name, range, defaultValue),
        sliderSmoothingSeconds(sliderSmoothingSeconds),
        smoothedValue(range.convertTo0to1(defaultValue)) {

}




float ModulatedParameterFloat::getModulated(const ModulationData& modulationData) {
    sharedData.functionCallStopwatch.stop();
    sharedData.modulationStopwatch.start();

    // TODO: Clipping
    float value = range.convertFrom0to1(getNormalized(modulationData));

    sharedData.modulationStopwatch.stop();
    sharedData.functionCallStopwatch.start();

    return value;
}

float ModulatedParameterFloat::getNormalizedBaseValue(const ModulationData &modulationData) {
    sharedData.modulationStopwatch.stop();
    sharedData.parameterStopwatch.start();

    float rawValue = juce::AudioParameterFloat::get();

    sharedData.parameterStopwatch.stop();
    sharedData.modulationStopwatch.start();

    smoothedValue.setTargetValue(range.convertTo0to1(rawValue));
    return smoothedValue.getNextValue();
}
