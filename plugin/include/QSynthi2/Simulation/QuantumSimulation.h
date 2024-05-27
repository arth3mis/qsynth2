#ifndef QUANTUMSIMULATION_H
#define QUANTUMSIMULATION_H

#include <complex>
#include "QSynthi2/Simulation/Simulation.h"
#include "QSynthi2/Simulation/Potential.h"

class QuantumSimulation final : public Simulation {
public:

    QuantumSimulation(int width, int height);
    ~QuantumSimulation() override;

    QuantumSimulation& addPotential(Potential p);
    QuantumSimulation& parabolaPotential(V2 offset, V2 factor);
    QuantumSimulation& barrierPotential(V2 pos, int width, const List<V2>& slits, Decimal value);
    QuantumSimulation& gaussianDistribution(V2 offset, V2 size, V2 impulse);

    void reset() override;

    // getters
    const ComplexMatrix& getNextFrame(Decimal timestep, const ModulationData& modulationData) override;

    [[nodiscard]] const List<RealMatrix>& getPotentials() const { return potentials; }
    [[nodiscard]] const ComplexMatrix& getPsi() const { return started ? psi : initialPsi; }
    [[nodiscard]] size_t getW() const { return W; }
    [[nodiscard]] size_t getH() const { return H; }

private:

    const size_t W, H;
    const float w, h;

    List<RealMatrix> potentials;
    ComplexMatrix potentialPrecalc;
    ComplexMatrix thetaPrecalc;
    ComplexMatrix initialPsi;
    ComplexMatrix psi;
    ComplexMatrix psiFFT;
    bool started;

    void calculateNextPsi(Decimal timestep);

    // returns pointer to psi for started simulation, else initialPsi
    ComplexMatrix* getPsiToChange() { return started ? &psi : &initialPsi; }

    // index to coordinates, normalized to [-1;1]
    [[nodiscard]] Decimal xOf(const size_t i) const { return (static_cast<Decimal>(i / W) - w/2) / (w/2); }  // NOLINT(*-integer-division)
    [[nodiscard]] Decimal yOf(const size_t i) const { return (static_cast<Decimal>(i % W) - h/2) / (h/2); }
    [[nodiscard]] size_t xIndexOf(const size_t i) const { return i / W; }
    [[nodiscard]] size_t yIndexOf(const size_t i) const { return i % W; }
    // coordinates [-1;1] to index
    [[nodiscard]] size_t toX(const Decimal cx) const { return std::max(0lu, std::min(W-1, static_cast<size_t>(cx * w/2 + w/2))); }
    [[nodiscard]] size_t toY(const Decimal cy) const { return std::max(0lu, std::min(H-1, static_cast<size_t>(cy * h/2 + h/2))); }
};

#endif //QUANTUMSIMULATION_H
