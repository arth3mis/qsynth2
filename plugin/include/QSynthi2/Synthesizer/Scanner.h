#pragma once


#include "QSynthi2/types.h"
#include "QSynthi2/Parameter/ParameterHolder.h"

class Scanner {

    float getPoint(num x, num y);


    void prepareToPlay(double sampleRate);


    void nextSample(ModulationData modulationData);
};