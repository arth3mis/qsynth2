#pragma once


#include "Parameters.h"
#include "QSynthi2/Parameter/ModulatedParameterFloat.h"

class ParameterCollection : public Parameters {
public:

    // Declare Parameters here
    ModulatedParameterFloat* timbre = add<ModulatedParameterFloat>("Timbre", juce::NormalisableRange<float>(0, 1), 0);

    // Add modulation slots at the end
    List<Modulation> modulations = addModulationSlots(8);

};