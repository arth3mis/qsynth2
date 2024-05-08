#ifndef QUANTUMSIMULATION_H
#define QUANTUMSIMULATION_H
#include <complex>

#include "QSynthi2/Simulation/Simulation.h"
#include "QSynthi2/List/Vector.h"

typedef float num;
typedef Vec2<num> V2;
typedef std::complex<num> cnum;
typedef List<cnum> CList;
typedef List<num> RList;
// typedef List<CList> CField;

class QuantumSimulation : Simulation<cnum> {
public:

    QuantumSimulation(int width, int height, num timestep);
    ~QuantumSimulation() override;

    QuantumSimulation& parabolaPotential(V2 offset, V2 factor);
    QuantumSimulation& barrierPotential(V2 pos, int width, num value);
    QuantumSimulation& gaussianDistribution(V2 offset, V2 size, V2 impulse);

    CList& getNextFrame() override;

    void reset();

private:

    const size_t W, H;
    const float w, h;
    CList initialPsi;
    RList potential;

    bool started;
    CList psi;
    CList psiP;
    num dt;

    void calculateNextPsi();

    // index to coordinates, normalized to [-1;1]
public:
    [[nodiscard]] num xOf(const size_t i) const { return (static_cast<num>(i / W) - w/2) / (w/2); }
    [[nodiscard]] num yOf(const size_t i) const { return (static_cast<num>(i % W) - h/2) / (h/2); }
    [[nodiscard]] size_t xIndexOf(const size_t i) const { return i / W; }
    [[nodiscard]] size_t yIndexOf(const size_t i) const { return i % W; }

    // for debug
    RList& getPotential() { return potential; }
    CList& getPsi() { return started ? psi : initialPsi; }
    [[nodiscard]] size_t getW() const { return W; }
    [[nodiscard]] size_t getH() const { return H; }
};

#endif //QUANTUMSIMULATION_H
