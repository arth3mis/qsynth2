#include "QSynthi2/Parameter/ModulatedParameterFloat.h"
#include "QSynthi2/Parameter/Parameters.h"
#include "QSynthi2/Data.h"

extern Data sharedData;


ModulatedParameterFloat::ModulatedParameterFloat() :
        juce::AudioParameterFloat(juce::ParameterID("", Parameters::VERSION), "", juce::NormalisableRange<float>(), 0) {

}

ModulatedParameterFloat::ModulatedParameterFloat(const juce::String &name, const juce::NormalisableRange<float> &range,
                                                 float defaultValue, Decimal sliderSmoothingSeconds) :
        juce::AudioParameterFloat(juce::ParameterID(name, Parameters::VERSION), name, range, defaultValue),
        sliderSmoothingSeconds(sliderSmoothingSeconds),
        smoothedNormalizedSliderValue(range.convertTo0to1(defaultValue)) {

}



void ModulatedParameterFloat::processBlock() {
    sharedData.functionCallStopwatch.stop();
    sharedData.modulationStopwatch.start();

    // Writes the raw value in the Buffer
    for (int i = 0; i < bufferNormalizedSliderValue.size(); ++i) {
        bufferNormalizedSliderValue[i] = smoothedNormalizedSliderValue.getNextValue();
    }

    sharedData.modulationStopwatch.stop();
    sharedData.functionCallStopwatch.start();
}



Eigen::ArrayX<Decimal> ModulatedParameterFloat::getModulatedNormalized(const ModulationData &modulationData) {
    Eigen::ArrayX<Decimal> buffer = bufferNormalizedSliderValue;

    for (auto modulation : modulations) {
        buffer = buffer + modulation->getModulatedNormalized(modulationData);
    }

    return buffer;
}



Eigen::ArrayX<Decimal> ModulatedParameterFloat::getModulated(const ModulationData &modulationData) {
    return getModulatedNormalized(modulationData).unaryExpr([this](Decimal decimal) {
        return static_cast<Decimal>(this->convertFrom0to1(static_cast<float>(decimal)));
    });
}


void ModulatedParameterFloat::valueChanged(float newValue) {
    smoothedNormalizedSliderValue.setTargetValue(static_cast<Decimal>(convertTo0to1(newValue)));
}
