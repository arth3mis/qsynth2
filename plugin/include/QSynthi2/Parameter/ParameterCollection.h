#pragma once


#include "Parameters.h"

class ParameterCollection : public Parameters {


    // Declare Parameters here
    const ModulatedParameterFloat* test = make<ModulatedParameterFloat>("TestParameter", juce::NormalisableRange<float>(0, 1), 0);



};