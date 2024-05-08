#pragma once

#include "ParameterAdapter.h"


class ParameterCollection : public ParameterAdapter {
public:

    ParameterCollection() : ParameterAdapter(1) { }

    const juce::String testParameterID = add("Test Parameter", 1000.f, juce::NormalisableRange<float>(20.f, 20000.f, 0.1f, .25f, false));


};