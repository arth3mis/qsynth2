#pragma once

#include "juce_audio_processors/juce_audio_processors.h"



class Stopwatch {
public:

    long nanoseconds;
    std::chrono::system_clock::time_point startTimePoint;
    bool isStarted = false;
    juce::String name;

    Stopwatch(juce::String name) : name(name) {

    }


    void start() {
        if (isStarted) return;
        startTimePoint = std::chrono::system_clock::now();
        isStarted = true;
    }

    void stop() {
        if (!isStarted) return;
        nanoseconds += (std::chrono::system_clock::now() - startTimePoint) / std::chrono::nanoseconds(1);
        isStarted = false;
    }

    void print() {
        juce::Logger::writeToLog(name + " took " + juce::String(nanoseconds) + "ns.");
    }

};