#ifndef QUANTUMSIMULATION_H
#define QUANTUMSIMULATION_H

#include <complex>
#include "QSynthi2/Simulation/Simulation.h"
#include "QSynthi2/Simulation/Potential.h"

class QuantumSimulationFrame final : public SimulationFrame {
public:
    explicit QuantumSimulationFrame(const ComplexMatrix& m) {
        frame = m;
    }
    [[nodiscard]] SimulationFrame* clone() override {
        return new QuantumSimulationFrame(frame);
    }
    [[nodiscard]] Decimal toDecimal(const long row, const long col) const override {
        if (row >= frame.rows() || col >= frame.cols())
            return 0;
        return std::abs(frame(row, col)) * 8; // todo tweak scaling
    }
    [[nodiscard]] Decimal toDecimalDisplay(const long row, const long col) const override {
        return std::pow(1.5 * toDecimal(row, col), 0.666);
    }
    [[nodiscard]] Decimal toPhase(const long row, const long col) const override {
        return std::arg(frame(row, col));
    }
    [[nodiscard]] size_t cols() const override {
        return static_cast<size_t>(frame.cols());
    }
    [[nodiscard]] size_t rows() const override {
        return static_cast<size_t>(frame.rows());
    }
private:
    ComplexMatrix frame;
};

class QuantumSimulation final : public Simulation {
public:

    QuantumSimulation(int width, int height);
    ~QuantumSimulation() override;

    QuantumSimulation& addPotential(Potential p);
    QuantumSimulation& linearPotential(Decimal angle, Decimal factor);
    QuantumSimulation& parabolaPotential(const V2& offset, const V2& factor);
    QuantumSimulation& barrierPotential(const V2&, int width, const List<V2>& slits, Decimal value);
    void resetGaussianDistribution(bool onlyApplyToInitialPsi = false);
    QuantumSimulation& gaussianDistribution(const V2& offset, const V2& size, const V2& impulse, bool onlyApplyToInitialPsi = false);

    void reset() override;
    void updateParameters(const ParameterCollection *p, const List<ModulationData*> &m) override;

    // getters
    SimulationFramePointer getStartFrame() override;
    SimulationFramePointer getNextFrame(Decimal timestep, const ModulationData& modulationData) override;
    bool isContinuous() override;
    bool isStationary() override;
    int getWidth() override { return static_cast<int>(W); }
    int getHeight() override { return static_cast<int>(H); }

    [[nodiscard]] const List<RealMatrix>& getPotentials() const { return potentials; }
    [[nodiscard]] const ComplexMatrix& getPsi() const { return started ? psi : initialPsi; }
    [[nodiscard]] Eigen::Index getW() const { return W; }
    [[nodiscard]] Eigen::Index getH() const { return H; }

private:

    const Eigen::Index W, H;
    const Decimal Wf, Hf;

    // parameters
    Decimal gaussianOffsetX = 0, gaussianOffsetY = 0;
    Decimal gaussianStretchX = 0, gaussianStretchY = 0;
    Decimal gaussianImpulseX = 0, gaussianImpulseY = 0;
    Decimal linearAngle = 0, linearFactor = 0;
    Decimal parabolaOffsetX = 0, parabolaOffsetY = 0;
    Decimal parabolaFactorX = 0, parabolaFactorY = 0;
    Decimal barrierOffsetX = 0;
    Decimal barrierWidth = 0;
    Decimal barrierSlit1Start = 0, barrierSlit1End = 0;
    Decimal barrierSlit2Start = 0, barrierSlit2End = 0;
    bool updateGaussian = false, updateLinear = false, updateParabola = false, updateBarrier = false;

    List<RealMatrix> potentials;
    // sorry, this program is not barrier-free!
    RealMatrix linearPotentialTemp;
    RealMatrix parabolaPotentialTemp;
    RealMatrix barrierPotentialTemp;
    RealMatrix barrierPotentialMask;
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
    [[nodiscard]] Decimal xOf(const Eigen::Index i) const { return (static_cast<Decimal>(i / W) - Wf/2) / (Wf/2); }  // NOLINT(*-integer-division)
    [[nodiscard]] Decimal yOf(const Eigen::Index i) const { return (static_cast<Decimal>(i % W) - Hf/2) / (Hf/2); }
    [[nodiscard]] Eigen::Index xIndexOf(const Eigen::Index i) const { return i / W; }
    [[nodiscard]] Eigen::Index yIndexOf(const Eigen::Index i) const { return i % W; }
    // coordinates [-1;1] to index
    [[nodiscard]] Eigen::Index toX(const Decimal cx) const { return std::max(static_cast<Eigen::Index>(0), std::min(W-1, static_cast<Eigen::Index>(cx * Wf/2 + Wf/2))); }
    [[nodiscard]] Eigen::Index toY(const Decimal cy) const { return std::max(static_cast<Eigen::Index>(0), std::min(H-1, static_cast<Eigen::Index>(cy * Hf/2 + Hf/2))); }
};

#endif //QUANTUMSIMULATION_H
