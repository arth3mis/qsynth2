#pragma once

#include <QSynthi2/types.h>
#include <QSynthi2/Parameter/ModulationData.h>

class ModulatedParameterFloat;

class Modulation {
public:

    Modulation();

    Modulation(size_t newModulationSourceId, ModulatedParameterFloat* newAmount);

    Eigen::ArrayX<Decimal> getModulatedNormalized(const ModulationData& modulationData);

    Decimal getSingleModulatedNormalized(const ModulationData& modulationData);

    void setModulatedParameterId(juce::String newModulatedParameterId);

    juce::String getModulatedParameterId();

    void setModulationSourceId(size_t newModulationSourceId);

    void setAmountParameter(ModulatedParameterFloat* newAmount);

    void prepareToPlay(int newSamplesPerBlock);

protected:

    int samplesPerBlock = 0;

    juce::String modulatedParameterId;
    size_t modulationSourceId;
    ModulatedParameterFloat* amount = nullptr;

};