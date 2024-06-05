#pragma once

#include "Scanner.h"
#include "QSynthi2/Parameter/ParameterCollection.h"

class Sonifier {
public:

    Sonifier();

    // TODO: Swappable implementation with timber sonifier
    Eigen::ArrayX<Decimal> generateNextBlock(const ModulationData& modulationData);

    void prepareToPlay(Decimal newSampleRate, int samplesPerBlock);

    void restart();



protected:

    Decimal phase0to1 = 0;

    Scanner scanner;

    Decimal sampleRate = 0;
    int samplesPerBlock = 0;
};