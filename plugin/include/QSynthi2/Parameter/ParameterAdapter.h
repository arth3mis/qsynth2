#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include <functional>
#include <utility>

class LambdaListener : public juce::AudioProcessorValueTreeState::Listener {
public:
    explicit LambdaListener(std::function<void(float)> _lambda) : lambda(std::move(_lambda)) { }

    void parameterChanged (const juce::String& parameterID, float newValue) override {
        lambda(newValue);
        juce::ignoreUnused(parameterID);
    }

private:
    std::function<void(float)> lambda;

};

class ParameterAdapter {
public:

    explicit ParameterAdapter(int _parameterVersion) : parameterVersion(_parameterVersion) { }

    // Call this in the Plugin Processors constructor. Finishes the parameter creation and connects to AudioProcessor
    void connectTo(juce::AudioProcessor &processorToConnectTo) {
        treeState = std::make_shared<juce::AudioProcessorValueTreeState>(
                processorToConnectTo,
                nullptr,
                "Parameters",
                std::move(layout));

        layoutInCreation = false;
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


    // Adds parameter to be added on the tree state
    juce::String add(const juce::String& parameterName, float defaultValue, const juce::NormalisableRange<float>& range) {
        jassert(layoutInCreation);
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { parameterName, parameterVersion }, parameterName, range, defaultValue));
        return parameterName;
    }


    // Call this to add listeners for a parameter. Adds a listener and calls it once with the parameters value (before loading the stateInformation).
    void addListener(const juce::String& parameterID, juce::AudioProcessorValueTreeState::Listener* listener) {
        jassert(listener != nullptr);
        jassert(!layoutInCreation); // Call connectTo() before adding listeners

        treeState->addParameterListener(parameterID, listener);

        // Call listener once
        listener->parameterChanged(parameterID, treeState->getRawParameterValue(parameterID)->load());
    }


    // Nicer with lambdas
    void addListener(const juce::String& parameterID, const std::function<void(float)>& lambda) {
        std::shared_ptr<LambdaListener> lambdaListener = std::make_shared<LambdaListener>(lambda);

        listeners[parameterID] = lambdaListener;
        addListener(parameterID, lambdaListener.get());
    }


protected:

    // Used for parameter ID creation
    int parameterVersion;

    // Tree state in which all parameters are stored
    std::shared_ptr<juce::AudioProcessorValueTreeState> treeState;

private:

    // Layout in that all Parameters are inserted
    // Gets filled after initialization. Is invalid to further use after connectTo() is called
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    bool layoutInCreation = true;

    // Map of registered Listeners for memory holding
    std::map<juce::String, std::shared_ptr<LambdaListener>> listeners;

};