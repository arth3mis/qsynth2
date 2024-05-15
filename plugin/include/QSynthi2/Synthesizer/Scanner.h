#pragma once


#include "QSynthi2/types.h"
#include "QSynthi2/Parameter/ParameterHolder.h"

class Scanner {
public:

    num getValueAt(num at) {
        // TODO
        return 0.0;
    }


    void prepareToPlay(num sampleRate) {
        timestep = 1 / sampleRate; // TODO getSimulationSpeed
    }


    void nextSample(ModulationData modulationData) {
        // TODO
    }

private:

    num time = 0;
    num timestep;

};