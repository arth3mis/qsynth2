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
    // Writes the raw value in the Buffer
    for (int i = 0; i < bufferNormalizedSliderValue.size(); ++i) {
        bufferNormalizedSliderValue[i] = smoothedNormalizedSliderValue.getNextValue();
    }
}



Eigen::ArrayX<Decimal> ModulatedParameterFloat::getModulatedNormalized(const ModulationData &modulationData) {
    Eigen::ArrayX<Decimal> buffer = bufferNormalizedSliderValue;

    for (auto modulation : modulations) {
        auto modulationValues = modulation->getModulatedNormalized(modulationData, static_cast<int>(buffer.size()));
        if (modulationValues.size() == 1) modulationValues = modulationValues.replicate(buffer.size(), 1);

        jassert(buffer.size() == modulationValues.size());
        buffer += modulationValues;
    }

    return buffer;
}


Eigen::ArrayX<Decimal> ModulatedParameterFloat::getModulatedNormalized(const ModulationData &modulationData, int samplesPerBlock) {
    if (samplesPerBlock == bufferNormalizedSliderValue.size()) return getModulatedNormalized(modulationData);
    jassert(samplesPerBlock > 0 && samplesPerBlock < bufferNormalizedSliderValue.size()); // Current implementation only works for numbers smaller than the buffer size

    auto buffer = Eigen::ArrayX<Decimal>(samplesPerBlock);

    for (long i = 0; i < samplesPerBlock; i++) {
        auto index = static_cast<long>(round(static_cast<Decimal>(i + 1) / static_cast<Decimal>(samplesPerBlock) * static_cast<Decimal>(bufferNormalizedSliderValue.size() - 1)));
        jassert(index >= 0 && index < bufferNormalizedSliderValue.size());
        buffer[i] = bufferNormalizedSliderValue[index];
    }

    for (auto modulation : modulations) {
        auto modulationValues = modulation->getModulatedNormalized(modulationData, static_cast<int>(buffer.size()));
        if (modulationValues.size() == 1) modulationValues = modulationValues.replicate(buffer.size(), 1);

        jassert(buffer.size() == modulationValues.size());
        buffer += modulationValues;
    }

    return buffer;
}



Eigen::ArrayX<Decimal> ModulatedParameterFloat::getModulated(const ModulationData &modulationData) {
    return getModulatedNormalized(modulationData).unaryExpr([this](Decimal decimal) {
        return static_cast<Decimal>(this->convertFrom0to1(static_cast<float>(decimal)));
    });
}



Eigen::ArrayX<Decimal> ModulatedParameterFloat::getModulated(const ModulationData &modulationData, int samplesPerBlock) {
    return getModulatedNormalized(modulationData, samplesPerBlock).unaryExpr([this](Decimal decimal) {
        return static_cast<Decimal>(this->convertFrom0to1(static_cast<float>(decimal)));
    });
}



Eigen::ArrayX<Decimal>ModulatedParameterFloat::getModulated(const List<ModulationData*> &modulationDataList, int samplesPerBlock) {
    auto buffer = Eigen::ArrayX<Decimal>(samplesPerBlock).setZero();
    auto weights = Eigen::ArrayX<Decimal>(samplesPerBlock).setZero();

    for (const auto& modulationData : modulationDataList) {
        weights += 1; // TODO: replace 1 with modulationData.at(Modulation::Sources::ENVELOPE1)
    }

    for (const auto& modulationData : modulationDataList) {
        buffer += getModulated(*modulationData, samplesPerBlock) * 1 / weights; // TODO: replace 1 with modulationData.at(Modulation::Sources::ENVELOPE1)
    }

    return buffer;
}



void ModulatedParameterFloat::valueChanged(float newValue) {
    smoothedNormalizedSliderValue.setTargetValue(static_cast<Decimal>(convertTo0to1(newValue)));
}
