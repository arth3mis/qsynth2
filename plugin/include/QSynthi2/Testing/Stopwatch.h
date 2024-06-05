#pragma once

#include "QSynthi2/Juce.h"

#define STOPWATCH_ACTIVE 1

class Stopwatch {
public:

    juce::String name;
    bool isStarted = false;
    long nanoseconds = 0;
    std::chrono::system_clock::time_point startTimePoint;

    explicit Stopwatch(const juce::String& displayName) : name(displayName) {
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

    [[nodiscard]] long get() const {
        if (nanoseconds == 0)
            return (std::chrono::system_clock::now() - startTimePoint) / std::chrono::nanoseconds(1);
        return nanoseconds;
    }

    void reset() {
        nanoseconds = 0;
        startTimePoint = std::chrono::system_clock::now();
    }

    void print(const long per=0, const juce::String& perName="") const {
#if STOPWATCH_ACTIVE==1
        juce::Logger::writeToLog(name + " took " + juce::String(nanoseconds/1000000) + " ms"
            // + (ref==0?"":" - "+juce::String(static_cast<double>(nanoseconds) / ref * 100, 2)) + (ref==0?"":"% of ref")
            + (per==0?"":" - "+juce::String(nanoseconds/1000000.0 / per, 4)) + (per==0?"":" ms per "+perName));
#endif
    }
};