#pragma once

#include <utility>

#include "juce_audio_processors/juce_audio_processors.h"
#include "Modulatable.h"
#include "FloatParameter.h"

class Modulation : public Modulatable {

public:

    Modulation(juce::String modulationSource, std::shared_ptr<FloatParameter> &amount) :
            modulationSource(std::move(modulationSource)), amount(amount) {


    }

    float getNormalizedBaseValue(const std::unordered_map<juce::String, float>& modulationData) override {
        return amount->getNormalized(modulationData);
    }

    float getNormalized(const std::unordered_map<juce::String, float>& modulationData) override {
        jassert(modulationData.contains(modulationSource)); // Tries to access not already calculated modulation value. Indicates cyclic modulation.
        jassert(!isnan(modulationData.at(modulationSource)));

        return Modulatable::getNormalized(modulationData) * modulationData.at(modulationSource);
    }


protected:

    juce::String modulationSource;
    std::shared_ptr<FloatParameter> amount;

};