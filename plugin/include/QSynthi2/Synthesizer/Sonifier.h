#pragma once


#include "Scanner.h"

class Sonifier {
public:

    Sonifier() : frequency(100) {

    }

    num getNextSample(const juce::MPENote& note, const ModulationData& modulationData) {
        scanner.nextSample();

        frequency.setTargetValue((num)note.getFrequencyInHertz());

        phase += frequency.getNextValue() / sampleRate;

        return scanner.getValueAt(phase, modulationData);
        // TODO: Implement
    }

    void prepareToPlay(num newSampleRate) {
        sampleRate = newSampleRate;

        frequency.reset(newSampleRate, 0.040);
    }

    void restart(const juce::MPENote& note) {
        frequency.setCurrentAndTargetValue((num)note.getFrequencyInHertz());

        scanner.restart();
    }



protected:

    Scanner scanner;

    num sampleRate;
    juce::SmoothedValue<num> frequency;

    num phase = 0;

};