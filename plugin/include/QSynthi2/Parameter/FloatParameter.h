#pragma once


#include "Modulation.h"

#include <utility>

class FloatParameter : public juce::AudioParameterFloat {

public:

    FloatParameter(const juce::String& name, int versionHint, const juce::NormalisableRange<float>& range, float defaultValue, float sliderSmoothingSeconds = 0.1f) :
            juce::AudioParameterFloat(juce::ParameterID(name, versionHint), name, range, defaultValue),
            sliderSmoothingSeconds(sliderSmoothingSeconds),
            smoothedValue(range.convertTo0to1(defaultValue)) {
    }


    /**
     * Assumed its only called once a sample
     * @param modulationData
     * @return
     */
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


    /**
     * Assumed its only called once a sample
     * @param modulationData
     * @return
     */
    float getModulated(const std::unordered_map<juce::String, float>& modulationData) {
        return range.convertFrom0to1(getNormalized(modulationData));
    }



    void prepareToPlay(double sampleRate) {
        smoothedValue.reset(sampleRate, sliderSmoothingSeconds);
    }


protected:

    float sliderSmoothingSeconds;
    juce::SmoothedValue<float> smoothedValue;

    List<Modulation> modulations;



};