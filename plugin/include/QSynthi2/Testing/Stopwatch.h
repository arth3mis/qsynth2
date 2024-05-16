#pragma once

#include "juce_audio_processors/juce_audio_processors.h"

#define STOPWATCH_ACTIVE 0

class Stopwatch {
public:

    long nanoseconds;
    std::chrono::system_clock::time_point startTimePoint;
    bool isStarted = false;
    juce::String name;

    Stopwatch(juce::String name) : name(name) {

    }


    void start() {
#if STOPWATCH_ACTIVE==1
        jassert(!isStarted);
        startTimePoint = std::chrono::system_clock::now();
        isStarted = true;
#endif
    }

    void stop() {
#if STOPWATCH_ACTIVE==1
        nanoseconds += (std::chrono::system_clock::now() - startTimePoint) / std::chrono::nanoseconds(1);
        isStarted = false;
#endif
    }

    void print() {
#if STOPWATCH_ACTIVE==1
        juce::Logger::writeToLog(name + " took " + juce::String(nanoseconds) + "ns.");
#endif
    }

};