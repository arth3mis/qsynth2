#pragma once

#include "QSynthi2/Juce.h"

#define STOPWATCH_ACTIVE 1

class Stopwatch {
public:

    long nanoseconds = 0;
    std::chrono::system_clock::time_point startTimePoint;
    bool isStarted = false;
    juce::String name;

    Stopwatch(juce::String displayName) : name(displayName) {

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
        if (nanoseconds == 0)
            return (std::chrono::system_clock::now() - startTimePoint) / std::chrono::nanoseconds(1);
        return nanoseconds;
    }

    void print(long per=0, juce::String perName="") {
#if STOPWATCH_ACTIVE==1
        juce::Logger::writeToLog(name + " took " + juce::String(nanoseconds/1000000) + " ms"
            // + (ref==0?"":" - "+juce::String(static_cast<double>(nanoseconds) / ref * 100, 2)) + (ref==0?"":"% of ref")
            + (per==0?"":" - "+juce::String(nanoseconds/1000000.0 / per, 4)) + (per==0?"":" ms per "+perName));
#endif
    }

};