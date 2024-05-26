
#include <utility>

#include "QSynthi2/Parameter/Modulation.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"
#include "QSynthi2/Data.h"


extern Data sharedData;



Modulation::Modulation() : modulationSource(Modulation::Sources::ALL[0]) {

}



Modulation::Modulation(juce::String modulationSource, ModulatedParameterFloat* amount):
    modulationSource(std::move(modulationSource)), amount(amount) {


}



Eigen::ArrayX<Decimal> Modulation::getModulatedNormalized(const ModulationData& modulationData) {
    // Return zeros if source isn't set.
    // TODO: is there a more performant way of doing this? Only if needed
    if (modulationSource.equalsIgnoreCase("")) return Eigen::ArrayX<Decimal>(samplesPerBlock).setZero();

    jassert(modulationData.contains(modulationSource)); // Invalid modulationSource. Isn't set in modulationData!
    jassert(modulationData.at(modulationSource).size() == amount->getModulated(modulationData).size()); // Block sizes arent equal
    return modulationData.at(modulationSource) * amount->getModulated(modulationData);
}



void Modulation::setModulatedParameterId(juce::String newModulatedParameterId) {
    modulatedParameterId = std::move(newModulatedParameterId);
}



juce::String Modulation::getModulatedParameterId() {
    return modulatedParameterId;
}



void Modulation::setModulationSource(juce::String newModulationSource) {
    modulationSource = std::move(newModulationSource);
}

void Modulation::setAmountParameter(ModulatedParameterFloat* newAmount) {
    amount = newAmount;
}

void Modulation::prepareToPlay(int newSamplesPerBlock) {
    samplesPerBlock = newSamplesPerBlock;
}