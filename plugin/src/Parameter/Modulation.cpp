
#include "QSynthi2/Parameter/Modulation.h"
#include "QSynthi2/Parameter/FloatParameter.h"

Modulation::Modulation(juce::String modulationSource, std::shared_ptr<FloatParameter> &amount):
    modulationSource(std::move(modulationSource)), amount(amount) {


}

float Modulation::getNormalizedBaseValue(const std::unordered_map<juce::String, float> &modulationData) {
    return amount->getNormalized(modulationData);
}

float Modulation::getNormalized(const std::unordered_map<juce::String, float> &modulationData) {
    jassert(modulationData.contains(modulationSource)); // Tries to access not already calculated modulation value. Indicates cyclic modulation.
    jassert(!std::isnan(modulationData.at(modulationSource)));

    float value = getNormalizedBaseValue(modulationData);

    for (auto& modulator : modulations) {
        value += modulator.getNormalized(modulationData);
    }

    return value * modulationData.at(modulationSource);
}
