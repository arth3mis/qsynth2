

#include "QSynthi2/Parameter/Modulatable.h"
#include "QSynthi2/Parameter/Modulation.h"


 Modulatable::~Modulatable() {}

float Modulatable::getNormalized(const std::unordered_map<juce::String, float>& modulationData) {
    float value = getNormalizedBaseValue(modulationData);

    for (auto& modulator : modulations) {
        value += modulator->getNormalized(modulationData);
    }

    return value;
}