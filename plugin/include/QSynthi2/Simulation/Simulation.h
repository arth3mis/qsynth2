#ifndef SIMULATION_H
#define SIMULATION_H

#include <complex>

#include "QSynthi2/types.h"
#include "QSynthi2/List/list.h"

typedef std::complex<num> SimNum;

class Simulation {
public:

    virtual ~Simulation() = default;

    virtual const List<SimNum>& getNextFrame(num timestep, const ModulationData& modulationData) = 0;
    virtual void reset() = 0;
};

#endif //SIMULATION_H
