#pragma once

#include "juce_audio_processors/juce_audio_processors.h"



class Stopwatch {
public:

    long nanoseconds;
    std::chrono::steady_clock::time_point startTimePoint;
    bool isStarted = false;
    juce::String name;

    Stopwatch(juce::String name) : name(name) {

    }


    void start() {
        jassert(!isStarted);
        startTimePoint = std::chrono::high_resolution_clock::now();
        isStarted = true;
    }

    void stop() {
        nanoseconds += (std::chrono::high_resolution_clock::now() - startTimePoint) / std::chrono::nanoseconds(1);
        isStarted = false;
    }

    void print() {
        juce::Logger::writeToLog(name + " took " + juce::String(nanoseconds) + "ns.");
    }

};