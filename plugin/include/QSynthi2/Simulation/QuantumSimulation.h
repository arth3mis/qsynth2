#ifndef QUANTUMSIMULATION_H
#define QUANTUMSIMULATION_H
#include <complex>
#include "QSynthi2/List/Vector.h"
#include "QSynthi2/Simulation/Simulation.h"

typedef float num;
typedef Vec2<num> V2;
typedef std::complex<num> cnum;
typedef List<cnum> CList;
typedef List<CList> CField;

class QuantumSimulation : Simulation<cnum> {
public:

    QuantumSimulation(int width, int height, num timestep);
    ~QuantumSimulation() override;

    QuantumSimulation& parabolaPotential(V2 offset, V2 factor);
    QuantumSimulation& barrierPotential(V2 pos, int width, num value);
    QuantumSimulation& gaussianDistribution(V2 offset, V2 relSize, V2 impulse);

    CList& getNextFrame() override;

private:

    const size_t W, H;
    const float w, h;
    CField initialPsi;
    CField potential;

    CList psi;
    num dt;

    void calculateNextPsi();

    // index to coordinates, normalized to [-1;1]
public:
    [[nodiscard]] num xOf(const size_t i) const { return (static_cast<num>(i / W) - w/2) / (w/2); }
    [[nodiscard]] num yOf(const size_t i) const { return (static_cast<num>(i % W) - h/2) / (h/2); }

    // for debug
    CField& getPotential() { return potential; }
    CList* getPsi() { return &psi; }
    [[nodiscard]] size_t getW() const { return W; }
    [[nodiscard]] size_t getH() const { return H; }
};

#endif //QUANTUMSIMULATION_H
