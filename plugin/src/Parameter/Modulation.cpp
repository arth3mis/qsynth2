
#include <utility>

#include "QSynthi2/Parameter/Modulation.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"
#include "QSynthi2/Data.h"


extern Data sharedData;



Modulation::Modulation() : modulationSourceId(static_cast<size_t>(0)) {

}



Modulation::Modulation(size_t modulationSourceId, ModulatedParameterFloat* amount):
    modulationSourceId(modulationSourceId), amount(amount) {


}



Eigen::ArrayX<Decimal> Modulation::getModulatedNormalized(const ModulationData& modulationData) {
    // Return zeros if source isn't set.
    // TODO: is there a more performant way of detecting this case? Only if needed
    if (modulationSourceId == static_cast<size_t>(-1)) return Eigen::ArrayXd::Constant(samplesPerBlock, 0);

    jassert(modulationSourceId < modulationData.size()); // Invalid modulationSource. Isn't set in modulationData!
    return modulationData[modulationSourceId] * amount->getModulated(modulationData);
}



Decimal Modulation::getSingleModulatedNormalized(const ModulationData &modulationData) {

    jassert(modulationData.size() > modulationSourceId); // Invalid modulationSource. Isn't set in modulationData!
    return modulationData[modulationSourceId](Eigen::last) * amount->getSingleModulated(modulationData);
}



void Modulation::setModulatedParameterId(juce::String newModulatedParameterId) {
    modulatedParameterId = std::move(newModulatedParameterId);
}



juce::String Modulation::getModulatedParameterId() {
    return modulatedParameterId;
}



void Modulation::setModulationSourceId(size_t newModulationSourceId) {
    modulationSourceId = newModulationSourceId;
}

void Modulation::setAmountParameter(ModulatedParameterFloat* newAmount) {
    amount = newAmount;
}

void Modulation::prepareToPlay(int newSamplesPerBlock) {
    samplesPerBlock = newSamplesPerBlock;
}
