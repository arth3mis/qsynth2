#pragma once
#include "QSynthi2/Juce.h"
#include "ModulatedParameterFloat.h"

class Parameters {

public:
    static const int VERSION = 1;


protected:
    // Tree state in which all parameters are stored
    std::shared_ptr<juce::AudioProcessorValueTreeState> treeState;

    juce::UndoManager undoManager;

    std::unordered_map<juce::String, ModulatedParameterFloat*> modulatedParameters;

    List<std::shared_ptr<Modulation>> modulations;

private:
    // Layout in that all Parameters are inserted
    // Gets filled after initialization. Is invalid to further use after connectTo() is called
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    bool layoutInCreation = true;



public:

    Parameters();

    // Call this in the Plugin Processors constructor. Finishes the parameter creation and connects to AudioProcessor
    void connectTo(juce::AudioProcessor &processorToConnectTo);

    // Call this to make new Parameters
    template<typename T, typename... Args>
    T *add(Args &&... args) {
        jassert(layoutInCreation); // Call this only before calling connectTo()!

        auto unique_pointer = std::make_unique<T>(std::forward<Args>(args)...);
        T& reference = *unique_pointer; // Save reference to object
        layout.add(std::move(unique_pointer));

        if (auto* modulatedParameterFloat = dynamic_cast<ModulatedParameterFloat*>(&reference)) {
            // Parameter is ModulatedParameterFloat
            modulatedParameters[modulatedParameterFloat->getParameterID()] = modulatedParameterFloat;
        }

        return &reference; // Return pointer to parameter
    }

    // Call this to add modulation slots for all before created parameters
    List<std::shared_ptr<Modulation>> &addModulationSlots(size_t number);

    // Call this in "getStateInformation(...)". Gives DAW Plugin data to store.
    void getStateInformation(juce::MemoryBlock& destData);

    // Call this in "setStateInformation(...)". Gets Parameter values to restore.
    void setStateInformation(const void* data, int sizeInBytes);

    // Call this in "prepareToPlay(...)". Setups buffer block sizes
    void prepareToPlay(Decimal sampleRate, int samplesPerBlock);

    // Call this in "processBlock(...)". Precalculates the parameter smoothing
    void processBlock();

};
