#pragma once


#include <QSynthi2/Simulation/QuantumSimulation.h>
#include <QSynthi2/Simulation/Simulation.h>

#include "QSynthi2/types.h"

class Scanner {
public:

    Scanner();

    num getValueAt(num at, const ModulationData& modulationData);


    void prepareToPlay(num newSampleRate);


    void nextSample();


    void restart();

private:

    num sampleRate = 0;

    size_t time{0};
    num timestep{0};

    std::unique_ptr<QuantumSimulation> sim;
    CList simFrameCurrent;
    CList simFramePrev;
};