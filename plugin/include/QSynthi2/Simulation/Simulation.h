#ifndef SIMULATION_H
#define SIMULATION_H

#include "QSynthi2/types.h"
#include "QSynthi2/List/list.h"
#include "QSynthi2/Parameter/ParameterHolder.h"

template <typename T>
class Simulation {
public:

    virtual ~Simulation() = default;

    virtual const List<T>& getNextFrame(num timestep, ModulationData modulationData) = 0;
};

#endif //SIMULATION_H
