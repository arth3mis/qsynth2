//
// Created by Jannis Müller on 14.05.24.
//
#include "QSynthi2/Parameter/ParameterHolder.h"

juce::AudioProcessorValueTreeState::ParameterLayout layout;

//template<typename T, typename... Args>
//T* ParameterHolder::add(Args &&... args) {
//    auto unique_pointer = std::make_unique<T>(std::forward<Args>(args)...);
//    T& reference = *unique_pointer; // Save reference to object
//    layout.add(std::move(unique_pointer));
//    return &reference; // Return pointer to parameter
//}

//template ModulatedParameterFloat* ParameterHolder::add(Args &&... args);
