#ifndef SIMULATION_H
#define SIMULATION_H

#include "QSynthi2/List/list.h"

template <typename T>
class Simulation {
public:

    virtual ~Simulation() = default;

    virtual List<T>& getNextFrame() = 0;
};

#endif //SIMULATION_H
