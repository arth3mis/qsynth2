#pragma once


#include "Parameters.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"

class ParameterCollection : public Parameters {
public:

    // Declare Parameters here
    ModulatedParameterFloat* modulationAmount = make<ModulatedParameterFloat>("ModAmount", juce::NormalisableRange<float>(0, 1), 0.7f);


    ModulatedParameterFloat* timbre = make<ModulatedParameterFloat>("Scanline Y", juce::NormalisableRange<float>(0, 1), 0.15f)
            //->withModulation(Modulation(Modulation::Sources::PRESSURE, modulationAmount))
            ->withModulation(Modulation(Modulation::Sources::TIMBRE, modulationAmount));


};