#pragma once


#include "Parameters.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"

class ParameterCollection : public Parameters {
public:

    // Declare Parameters here
    ModulatedParameterFloat* modulationAmount = make<ModulatedParameterFloat>("ModAmount", juce::NormalisableRange<float>(0, 1), 0.2);


    ModulatedParameterFloat* timbre = make<ModulatedParameterFloat>("Timbre", juce::NormalisableRange<float>(0, 1), 0)
            ->withModulation(Modulation(Modulation::Sources::PRESSURE, modulationAmount))
            ->withModulation(Modulation(Modulation::Sources::TIMBRE, modulationAmount));


};