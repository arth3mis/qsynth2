#pragma once

#include "QSynthi2/types.h"

class Scanner {
public:

    Scanner();

    Decimal getValueAt(Decimal at, const ModulationData& modulationData);
    void prepareToPlay(Decimal newSampleRate);
    void nextSample();
    void restart();

private:

    Decimal sampleRate = 0;

    size_t frameCounter;    // todo knows needed frame index

};