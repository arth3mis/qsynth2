#pragma once


#include "Modulation.h"
#include "QSynthi2/AudioProcessing/AJAudioProcessor.h"

#include <utility>

class ModulatedParameterFloat : public juce::AudioParameterFloat {

public:

    ModulatedParameterFloat();

    ModulatedParameterFloat(const juce::String& name, const juce::NormalisableRange<float>& range, float defaultValue, float sliderSmoothingSeconds = 0.1f);


    float getNormalizedBaseValue(const ModulationData& modulationData);


    float getNormalized(const ModulationData &modulationData) {
        float value = getNormalizedBaseValue(modulationData);

        /*
        for (auto& modulator : modulations) {
            value += modulator.getNormalized(modulationData);
        }
         */

        for (size_t i = 0; i < modulations.size(); ++i) {
            value += modulations[i].getNormalized(modulationData);
        }

        return value;
    }


    float getModulated(const ModulationData& modulationData);



    void prepareToPlay(double sampleRate) {
        smoothedValue.reset(sampleRate, sliderSmoothingSeconds);
    }


    ModulatedParameterFloat* withModulation(const Modulation& modulation) {
        modulations.append(modulation);
        return this;
    }



protected:

    float sliderSmoothingSeconds;
    juce::SmoothedValue<float> smoothedValue;

    List<Modulation> modulations;



};