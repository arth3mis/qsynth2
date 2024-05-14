//
// Created by art on 14.05.24.
//

#ifndef POTENTIAL_H
#define POTENTIAL_H

#include "QSynthi2/Simulation/QuantumSimulationTypes.h"

enum class PotentialType {
    PARABOLA, BARRIER
};

class Potential {
public:
    explicit Potential(const PotentialType pt) : type(pt) {

    }

private:
    const PotentialType type;

    RList values;
};

class ParabolaPotential : public Potential {
public:
    ParabolaPotential(V2 offset, V2 factor) : Potential(PotentialType::PARABOLA) {

    }
};

#endif //POTENTIAL_H
