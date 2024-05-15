#pragma once


#include "QSynthi2/types.h"

class Scanner {
public:

    num getValueAt(num at);


    void prepareToPlay(num sampleRate);


    void nextSample(ModulationData modulationData);

private:

    num time = 0;
    num timestep;

};