#ifndef SIMULATION_H
#define SIMULATION_H

#include "QSynthi2/types.h"

class Simulation {
public:

    virtual ~Simulation() = default;

    virtual const ComplexMatrix& getNextFrame(Decimal timestep, const ModulationData& modulationData) = 0;
    virtual void reset() = 0;
};

#endif //SIMULATION_H
