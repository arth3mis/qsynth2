#ifndef SIMULATION_H
#define SIMULATION_H

#include <QSynthi2/Parameter/ParameterCollection.h>

#include "QSynthi2/types.h"
#include "QSynthi2/Parameter/ModulationData.h"

class Simulation {
public:

    virtual ~Simulation() = default;

    virtual SimulationFramePointer getStartFrame() = 0;
    virtual SimulationFramePointer getNextFrame(Decimal timestep, const ModulationData& modulationData) = 0;
    virtual void reset() = 0;
    virtual void updateParameters(const ParameterCollection* parameterCollection, const List<ModulationData*> &modulationDataList) = 0;
};

#endif //SIMULATION_H
