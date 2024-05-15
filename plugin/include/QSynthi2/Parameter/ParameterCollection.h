#pragma once


#include "Parameters.h"

class ParameterCollection : public Parameters {
public:

    // Declare Parameters here
    ModulatedParameterFloat* modulationAmount = make<ModulatedParameterFloat>("ModAmount", juce::NormalisableRange<float>(0, 1), 0);


    Modulation modulation = Modulation(Modulation::Sources::TIMBRE, modulationAmount);

    ModulatedParameterFloat* timbre = make<ModulatedParameterFloat>("Timbre", juce::NormalisableRange<float>(0, 1), 0)
            ->withModulation(modulation);


};