#pragma once

#include "Modulation.h"
#include "QSynthi2/AudioProcessing/AJAudioProcessor.h"

class ModulatedParameterFloat : public juce::AudioParameterFloat {
public:

    ModulatedParameterFloat();

    ModulatedParameterFloat(const juce::String& name, const juce::NormalisableRange<float>& range, float defaultValue, Decimal sliderSmoothingSeconds = 0.1);


    void processBlock();

    Eigen::ArrayX<Decimal> getModulatedNormalized(ModulationData &modulationData);

    Eigen::ArrayX<Decimal> getModulated(ModulationData &modulationData);

    // TODO: With given Array of ModulationData. Weighted Average by Envelope 1?

    void prepareToPlay(Decimal sampleRate, int samplesPerBlock) {
        smoothedNormalizedSliderValue.reset(sampleRate, sliderSmoothingSeconds);
        bufferNormalizedSliderValue = Eigen::ArrayX<Decimal>(samplesPerBlock);
    }

    ModulatedParameterFloat* withModulation(Modulation modulation) {
        modulations.append(modulation);
        return this;
    }

protected:

    Eigen::ArrayX<Decimal> bufferNormalizedSliderValue;

    Decimal sliderSmoothingSeconds;
    juce::SmoothedValue<Decimal> smoothedNormalizedSliderValue;

    List<Modulation> modulations;


    // Listener
    void valueChanged(float newValue) override;

};