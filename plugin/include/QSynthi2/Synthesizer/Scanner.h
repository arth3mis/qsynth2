#pragma once

#include "QSynthi2/types.h"
#include "QSynthi2/Parameter/ParameterCollection.h"

class Scanner {
public:

    Scanner();

    Eigen::ArrayXX<Decimal> getValuesAt(const Eigen::ArrayXX<Decimal> &position0to1, const ModulationData &modulationData);

    void prepareToPlay(Decimal newSampleRate);

    void restart();

private:

    Decimal sampleRate;

    size_t frameCounter;    // todo knows needed frame index

};