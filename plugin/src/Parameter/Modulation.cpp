
#include "QSynthi2/Parameter/Modulation.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"

Modulation::Modulation(juce::String modulationSource, ModulatedParameterFloat* amount):
    modulationSource(std::move(modulationSource)), amount(amount) {


}

float Modulation::getNormalizedBaseValue(const ModulationData &modulationData) {
    return amount->getNormalized(modulationData);
}

float Modulation::getNormalized(const ModulationData &modulationData) {
    jassert(modulationData.contains(modulationSource)); // Tries to access not already calculated modulation value. Indicates cyclic modulation.
    jassert(!std::isnan(modulationData.at(modulationSource)));

    float value = getNormalizedBaseValue(modulationData);

    for (auto& modulator : modulations) {
        value += modulator.getNormalized(modulationData);
    }

    return value * modulationData.at(modulationSource);
}
