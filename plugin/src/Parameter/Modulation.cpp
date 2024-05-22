
#include "QSynthi2/Parameter/Modulation.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"
#include "QSynthi2/Data.h"


extern Data sharedData;

Modulation::Modulation(juce::String modulationSource, ModulatedParameterFloat* amount):
    modulationSource(std::move(modulationSource)), amount(amount) {


}

float Modulation::getNormalizedBaseValue(const ModulationData &modulationData) {
    return amount->getModulated(modulationData);
}

float Modulation::getNormalized(const ModulationData &modulationData) {
    jassert(modulationData.contains(modulationSource)); // Tries to access not already calculated modulation value. Indicates cyclic modulation.
    jassert(!std::isnan(modulationData.at(modulationSource)));

    float value = getNormalizedBaseValue(modulationData);

    for (auto& modulator : modulations) {
        value += modulator.getNormalized(modulationData);
    }

    sharedData.modulationStopwatch.stop();
    sharedData.hashMapStopwatch.start();

    value = value * (float)modulationData.at(modulationSource);

    sharedData.hashMapStopwatch.stop();
    sharedData.modulationStopwatch.start();

    return value;
}
