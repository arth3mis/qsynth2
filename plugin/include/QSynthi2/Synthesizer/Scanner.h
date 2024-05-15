#pragma once


#include "QSynthi2/types.h"

class Scanner {
public:

    num getValueAt(num at, const ModulationData& modulationData);


    void prepareToPlay(num sampleRate);


    void nextSample();


    void restart();

private:

    num time = 0;
    num timestep;

};