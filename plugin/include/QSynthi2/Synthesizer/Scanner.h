#pragma once


#include <QSynthi2/Simulation/QuantumSimulation.h>
#include <QSynthi2/Simulation/Simulation.h>

#include "QSynthi2/types.h"

class Scanner {
public:

    Scanner(const std::shared_ptr<Simulation>& simRef);

    num getValueAt(num at, const ModulationData& modulationData);
    void prepareToPlay(num newSampleRate);
    void nextSample();
    void restart();

private:

    num sampleRate = 0;

    size_t time{0};
    num timestep{0};

    std::shared_ptr<Simulation> sim;
    size_t frameCounter;    // todo knows needed frame index
    CSimMatrix simFrameCurrent;
    CSimMatrix simFramePrev;
};