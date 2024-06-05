#pragma once

#include "Modulation.h"

class ModulatedParameterFloat : public juce::AudioParameterFloat {
public:

    List<std::shared_ptr<Modulation>> modulations;



    ModulatedParameterFloat();

    ModulatedParameterFloat(const juce::String& name, const juce::NormalisableRange<float>& range, float defaultValue, Decimal sliderSmoothingSeconds = 0.1);


    void processBlock();

    Eigen::ArrayX<Decimal> getModulatedNormalized(const ModulationData &modulationData);

    Eigen::ArrayX<Decimal> getModulatedNormalized(const ModulationData &modulationData, int samplesPerBlock);

    Eigen::ArrayX<Decimal> getModulated(const ModulationData &modulationData);

    Eigen::ArrayX<Decimal> getModulated(const ModulationData &modulationData, int samplesPerBlock);

    Eigen::ArrayX<Decimal> getModulated(const List<ModulationData*> &modulationData, int samplesPerBlock);

    // TODO: With given Array of ModulationData. Weighted Average by Envelope #1?
    // and number of wanted samples

    void prepareToPlay(Decimal sampleRate, int samplesPerBlock) {
        smoothedNormalizedSliderValue.reset(sampleRate, sliderSmoothingSeconds);
        bufferNormalizedSliderValue = Eigen::ArrayX<Decimal>(samplesPerBlock);
    }

    ModulatedParameterFloat* withModulation(const std::shared_ptr<Modulation>& modulation) {
        modulations.append(modulation);
        return this;
    }

protected:

    Eigen::ArrayX<Decimal> bufferNormalizedSliderValue;

    Decimal sliderSmoothingSeconds;
    juce::SmoothedValue<Decimal> smoothedNormalizedSliderValue;


    // Listener
    void valueChanged(float newValue) override;

};