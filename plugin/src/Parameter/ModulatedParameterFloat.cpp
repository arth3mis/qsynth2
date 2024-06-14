#include "QSynthi2/Parameter/ModulatedParameterFloat.h"
#include "QSynthi2/Parameter/Parameters.h"
#include "QSynthi2/Data.h"

extern Data sharedData;


ModulatedParameterFloat::ModulatedParameterFloat() :
        juce::AudioParameterFloat(juce::ParameterID("", Parameters::VERSION), "", juce::NormalisableRange<float>(), 0) {

}

ModulatedParameterFloat::ModulatedParameterFloat(const juce::String &newName, const juce::NormalisableRange<float> &newRange,
                                                 float defaultValue, Decimal sliderSmoothingSeconds) :
        juce::AudioParameterFloat(juce::ParameterID(newName, Parameters::VERSION), newName, newRange, defaultValue),
        sliderSmoothingSeconds(sliderSmoothingSeconds),
        smoothedNormalizedSliderValue(newRange.convertTo0to1(defaultValue)) {

}



void ModulatedParameterFloat::processBlock() {
    Decimal currentValue = smoothedNormalizedSliderValue.getCurrentValue();
    Decimal targetValue = smoothedNormalizedSliderValue.getTargetValue();
    if (juce::approximatelyEqual(currentValue, targetValue) && juce::approximatelyEqual(bufferNormalizedSliderValue[0], targetValue)) {
        // Buffer would not really change
        return;
    }

    // Writes the raw value in the Buffer
    for (double & i : bufferNormalizedSliderValue) {
        i = smoothedNormalizedSliderValue.getNextValue();
    }
}


Eigen::ArrayX<Decimal> ModulatedParameterFloat::convertFrom0to1(const Eigen::ArrayX<Decimal> &values0to1) {
    return values0to1.unaryExpr([this](Decimal decimal) {
        return static_cast<Decimal>(juce::AudioParameterFloat::convertFrom0to1(static_cast<float>(decimal)));
    });;
}



Eigen::ArrayX<Decimal> ModulatedParameterFloat::getModulatedNormalized(const ModulationData &modulationData) {
    Eigen::ArrayX<Decimal> buffer = bufferNormalizedSliderValue;

    for (const auto& modulation : modulations) {
        auto modulationValues = modulation->getModulatedNormalized(modulationData);

        jassert(buffer.size() == modulationValues.size());
        buffer += modulationValues;
    }

    return buffer;
}



Decimal ModulatedParameterFloat::getSingleModulatedNormalized(const ModulationData &modulationData) {
    Decimal buffer = bufferNormalizedSliderValue(Eigen::last);

    for (const auto& modulation : modulations) {
        buffer += modulation->getSingleModulatedNormalized(modulationData);
    }

    return buffer;
}



Eigen::ArrayX<Decimal> ModulatedParameterFloat::getModulated(const ModulationData &modulationData) {
    return convertFrom0to1(getModulatedNormalized(modulationData));
}



Eigen::ArrayX<Decimal> ModulatedParameterFloat::getModulated(const List<ModulationData *> &modulationDataList) {
    auto buffer = bufferNormalizedSliderValue;

    for (const auto& modulationData : modulationDataList) {
        for (const auto& modulation : modulations) {
            auto modulationValues = modulation->getModulatedNormalized(*modulationData);

            jassert(buffer.size() == modulationValues.size());
            buffer += modulationValues * modulationData->atSource(ModulationData::Sources::ENVELOPE1);
        }
    }

    return convertFrom0to1(buffer);
}



Decimal ModulatedParameterFloat::getSingleModulated(const ModulationData &modulationData) {
    return static_cast<Decimal>(juce::AudioParameterFloat::convertFrom0to1(static_cast<float>(getSingleModulatedNormalized(modulationData))));
}





Decimal ModulatedParameterFloat::getSingleModulated(const List<ModulationData *> &modulationDataList) {
    Decimal buffer = bufferNormalizedSliderValue(Eigen::last);

    for (const auto& modulationData : modulationDataList) {
        for (const auto &modulation: modulations) {
            buffer += modulation->getSingleModulatedNormalized(*modulationData) * modulationData->atSource(ModulationData::Sources::ENVELOPE1)(Eigen::last);
        }
    }


    return static_cast<Decimal>(juce::AudioParameterFloat::convertFrom0to1(static_cast<float>(buffer)));
}




void ModulatedParameterFloat::valueChanged(float newValue) {
    smoothedNormalizedSliderValue.setTargetValue(static_cast<Decimal>(convertTo0to1(newValue)));
}