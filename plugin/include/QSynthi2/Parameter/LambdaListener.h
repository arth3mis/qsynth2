#pragma once

#include "QSynthi2/types.h"


class LambdaListener : public juce::AudioProcessorParameter::Listener
{
public:

    LambdaListener(std::function<void(float)> callback) : callback(callback) { }

    void parameterValueChanged(int parameterIndex, float newValue) override {
        juce::ignoreUnused(parameterIndex);
        callback(newValue);
    }

    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {
        juce::ignoreUnused(parameterIndex, gestureIsStarting);
    }

private:
    std::function<void(float)> callback;
};