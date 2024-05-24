
#include "QSynthi2/Parameter/Modulation.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"
#include "QSynthi2/Data.h"


extern Data sharedData;

Modulation::Modulation(juce::String modulationSource, ModulatedParameterFloat* amount):
    modulationSource(std::move(modulationSource)), amount(amount) {


}



const Eigen::ArrayX<Decimal> Modulation::getModulatedNormalized(ModulationData& modulationData) {
    jassert(modulationData.contains(modulationSource)); // Modulation Source is not set
    jassert(modulationData[modulationSource].size() == amount->getModulatedNormalized(modulationData).size()); // Block sizes arent equal
    return modulationData[modulationSource] * amount->getModulatedNormalized(modulationData);
}