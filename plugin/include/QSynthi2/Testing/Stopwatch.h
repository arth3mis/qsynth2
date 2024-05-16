#pragma once

#include "juce_audio_processors/juce_audio_processors.h"

#define STOPWATCH_ACTIVE 1

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
        if (isStarted)
            return;
        startTimePoint = std::chrono::system_clock::now();
        isStarted = true;
#endif
    }

    void stop() {
#if STOPWATCH_ACTIVE==1
        if (!isStarted)
            return;
        nanoseconds += (std::chrono::system_clock::now() - startTimePoint) / std::chrono::nanoseconds(1);
        isStarted = false;
#endif
    }

    long get() {
        return nanoseconds;
    }

    void print(long ref=0, long per=0) {
#if STOPWATCH_ACTIVE==1
        juce::Logger::writeToLog(name + " took " + juce::String(nanoseconds/1000000)
            + " ms (" + (ref==0?"":juce::String(static_cast<double>(nanoseconds) / ref * 100, 2))
            + (ref==0?")":"% of ref / ")
            + (per==0?"":" "+juce::String(nanoseconds/1000000.0 / per, 4)) + (per==0?"":" ms per timestep)."));
#endif
    }

};