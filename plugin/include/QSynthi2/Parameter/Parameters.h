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

private:
    // Layout in that all Parameters are inserted
    // Gets filled after initialization. Is invalid to further use after connectTo() is called
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    bool layoutInCreation = true;



public:

    Parameters() { }

    // Call this in the Plugin Processors constructor. Finishes the parameter creation and connects to AudioProcessor
    void connectTo(juce::AudioProcessor &processorToConnectTo) {
        jassert(layoutInCreation);
        layoutInCreation = false;

        treeState = std::make_shared<juce::AudioProcessorValueTreeState>(
                processorToConnectTo,
                nullptr,
                "Parameters",
                std::move(layout));

    }


    template <typename T, typename... Args>
    T* make(Args&&... args) {
        jassert(layoutInCreation);

        auto unique_pointer = std::make_unique<T>(std::forward<Args>(args)...);
        T& reference = *unique_pointer; // Save reference to object
        layout.add(std::move(unique_pointer));
        return &reference; // Return pointer to parameter
    }


    // Call this in "getStateInformation(...)". Gives DAW Plugin data to store.
    void getStateInformation(juce::MemoryBlock& destData) {
        juce::MemoryOutputStream memoryStream(destData, true);
        treeState->state.writeToStream(memoryStream);
    }


    // Call this in "setStateInformation(...)". Gets Parameter values to restore.
    void setStateInformation(const void* data, int sizeInBytes) {
        auto inputTree = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes));
        if (inputTree.isValid()) {
            treeState->replaceState(inputTree);
        }

    }

};