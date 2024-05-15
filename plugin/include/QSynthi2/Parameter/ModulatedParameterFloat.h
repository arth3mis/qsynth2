#pragma once


#include "Modulation.h"

#include <utility>

class ModulatedParameterFloat : public juce::AudioParameterFloat {

public:

    ModulatedParameterFloat();

    ModulatedParameterFloat(const juce::String& name, const juce::NormalisableRange<float>& range, float defaultValue, float sliderSmoothingSeconds = 0.1f);


    float getNormalizedBaseValue(const ModulationData& modulationData) {
        smoothedValue.setTargetValue(range.convertTo0to1(juce::AudioParameterFloat::get()));
        return smoothedValue.getNextValue();
    }


    float getNormalized(const ModulationData &modulationData) {
        float value = getNormalizedBaseValue(modulationData);

        for (auto& modulator : modulations) {
            value += modulator.getNormalized(modulationData);
        }

        return value;
    }


    float getModulated(const ModulationData& modulationData) {
        return range.convertFrom0to1(getNormalized(modulationData));
    }



    void prepareToPlay(double sampleRate) {
        smoothedValue.reset(sampleRate, sliderSmoothingSeconds);
    }


    ModulatedParameterFloat* withModulation(Modulation& modulation) {
        modulations.append(modulation);
        return this;
    }



protected:

    float sliderSmoothingSeconds;
    juce::SmoothedValue<float> smoothedValue;

    List<Modulation> modulations;



};