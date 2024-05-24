#pragma once

#include "Scanner.h"

class Sonifier {
public:

    Sonifier() {
    }

    // TODO: Swappable implementation with timber sonifier
    Eigen::ArrayX<Decimal> generateNextBlock(const ModulationData& modulationData) {
        jassert(modulationData.contains(Modulation::Sources::PITCH)); // Pitch isn't already evaluated
        auto frequency = modulationData.at(Modulation::Sources::PITCH);
        jassert(frequency.size() == samplesPerBlock); // Pitch wasn't set properly

        auto phases = Eigen::ArrayX<Decimal>(samplesPerBlock);

        for(int i = 0; i < phases.size(); i++) {
            phase0to1 += frequency[i] / sampleRate;
            phase0to1 = fmod(phase0to1, 1);

            phases[i] = phase0to1;
        }

        return scanner.getValuesAt(phases, modulationData);
    }

    void prepareToPlay(Decimal newSampleRate, int samplesPerBlock) {
        sampleRate = newSampleRate;
        this->samplesPerBlock = samplesPerBlock;

        scanner.prepareToPlay(newSampleRate);
    }

    void restart() {
        phase0to1 = 0;
        scanner.restart();
    }



protected:

    Decimal phase0to1;

    Scanner scanner;

    Decimal sampleRate;
    int samplesPerBlock;
};