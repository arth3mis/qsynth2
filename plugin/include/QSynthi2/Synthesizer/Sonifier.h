#pragma once

#include "Scanner.h"

class Sonifier {
public:

    Sonifier()
        : frequency(100) {
    }

    Decimal getNextSample(const juce::MPENote& note, const ModulationData& modulationData) {
        scanner.nextSample();

        frequency.setTargetValue((Decimal)note.getFrequencyInHertz());

        phase += frequency.getNextValue() / sampleRate;
        phase = std::fmod(phase, 1);

        return scanner.getValueAt(phase, modulationData);
        // TODO: Implement
    }

    void prepareToPlay(Decimal newSampleRate) {
        sampleRate = newSampleRate;

        frequency.reset(newSampleRate, 0.040);

        scanner.prepareToPlay(newSampleRate);
    }

    void restart(const juce::MPENote& note) {
        frequency.setCurrentAndTargetValue((Decimal)note.getFrequencyInHertz());

        scanner.restart();
    }



protected:

    Scanner scanner;

    Decimal sampleRate;
    juce::SmoothedValue<Decimal> frequency;

    Decimal phase = 0;

};