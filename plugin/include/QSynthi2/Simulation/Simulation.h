#ifndef SIMULATION_H
#define SIMULATION_H

#include "QSynthi2/types.h"
#include "QSynthi2/List/list.h"


class Simulation {
public:

    virtual ~Simulation() = default;

    virtual const CSimMatrix& getNextFrame(num timestep, const ModulationData& modulationData) = 0;
    virtual void reset() = 0;

private:
};

#endif //SIMULATION_H
