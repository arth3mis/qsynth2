#pragma once

#include "Modulation.h"

class ModulatedParameterFloat : public juce::AudioParameterFloat {
public:

    List<std::shared_ptr<Modulation>> modulations;



    ModulatedParameterFloat();

    ModulatedParameterFloat(const juce::String& name, const juce::NormalisableRange<float>& range, float defaultValue, Decimal sliderSmoothingSeconds = 0.1);


    void processBlock();

    Eigen::ArrayX<Decimal> convertFrom0to1(const Eigen::ArrayX<Decimal> &values0to1);

    Eigen::ArrayX<Decimal> getModulatedNormalized(const ModulationData &modulationData);

    Decimal getSingleModulatedNormalized(const ModulationData &modulationData);

    Eigen::ArrayX<Decimal> getModulated(const ModulationData &modulationData);

    Eigen::ArrayX<Decimal> getModulated(const List<ModulationData*> &modulationDataList);

    Decimal getSingleModulated(const ModulationData &modulationData);

    Decimal getSingleModulated(const List<ModulationData*> &modulationDataList);

    void prepareToPlay(Decimal sampleRate, int samplesPerBlock);

    ModulatedParameterFloat* withModulation(const std::shared_ptr<Modulation>& modulation);

protected:

    Eigen::ArrayX<Decimal> bufferNormalizedSliderValue;

    Decimal sliderSmoothingSeconds{0.1};
    juce::SmoothedValue<Decimal> smoothedNormalizedSliderValue;


    // Listener
    void valueChanged(float newValue) override;

};