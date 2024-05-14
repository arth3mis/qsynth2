#ifndef QUANTUMSIMULATION_H
#define QUANTUMSIMULATION_H
#include <complex>

#include "QSynthi2/Simulation/QuantumSimulationTypes.h"
#include "QSynthi2/Simulation/Simulation.h"
#include "QSynthi2/Simulation/Potential.h"


class QuantumSimulation final : Simulation<cnum> {
public:

    QuantumSimulation(int width, int height, num timestep);
    ~QuantumSimulation() override;

    QuantumSimulation& potential(Potential p);
    QuantumSimulation& parabolaPotential(V2 offset, V2 factor);
    QuantumSimulation& barrierPotential(V2 start, V2 end, int width, num value);
    QuantumSimulation& gaussianDistribution(V2 offset, V2 size, V2 impulse);

    void reset();

    // getters
    const CList& getNextFrame() override;

    [[nodiscard]] const List<RList>& getPotentials() const { return potentials; }
    [[nodiscard]] const CList& getPsi() const { return started ? psi : initialPsi; }
    [[nodiscard]] size_t getW() const { return W; }
    [[nodiscard]] size_t getH() const { return H; }

private:

    const size_t W, H;
    const float w, h;

    List<RList> potentials;
    CList initialPsi;
    CList psi;
    CList psiP;
    bool started;
    num dt;

    void calculateNextPsi();

    // index to coordinates, normalized to [-1;1]
    [[nodiscard]] num xOf(const size_t i) const { return (static_cast<num>(i / W) - w/2) / (w/2); }
    [[nodiscard]] num yOf(const size_t i) const { return (static_cast<num>(i % W) - h/2) / (h/2); }
    [[nodiscard]] size_t xIndexOf(const size_t i) const { return i / W; }
    [[nodiscard]] size_t yIndexOf(const size_t i) const { return i % W; }
};

#endif //QUANTUMSIMULATION_H
