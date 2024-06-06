#include "QSynthi2/Simulation/QuantumSimulation.h"
#include "QSynthi2/Juce.h"
#include "QSynthi2/FFT.h"
#include <cmath>

QuantumSimulation::QuantumSimulation(const int width, const int height)
    : Simulation()
    , W(width)
    , H(height)
    , w(static_cast<Decimal>(width))
    , h(static_cast<Decimal>(height)) {
    initialPsi = ComplexMatrix::Zero(H, W);
    psi = ComplexMatrix::Zero(H, W);
    psiFFT = ComplexMatrix::Zero(H, W);
    thetaPrecalc = ComplexMatrix(H, W);

    started = false;

    constexpr Decimal pi2 = juce::MathConstants<Decimal>::twoPi;
    for (int i = 0; i < W; ++i) {
        for (int j = 0; j < H; ++j) {
            const Decimal k = pi2 * std::min(static_cast<Decimal>(i), w-static_cast<Decimal>(i)) / w;
            const Decimal l = pi2 * std::min(static_cast<Decimal>(j), h-static_cast<Decimal>(j)) / h;
            const Decimal theta = (k*k + l*l);
            thetaPrecalc(j, i) = theta * Complex(0, 1);
        }
    }
}

QuantumSimulation::~QuantumSimulation() = default;

QuantumSimulation& QuantumSimulation::addPotential(const Potential p) {
    // potentials.push_back(p);
    potentialPrecalc = potentials.sum() * Complex(0, 1);
    return *this;
}

QuantumSimulation& QuantumSimulation::parabolaPotential(const V2 offset, const V2 factor) {
    const size_t h = potentials.append(RealMatrix::Zero(H, W));
    for (int i = 0; i < W * H; ++i) {
        const Decimal x = xOf(i) - offset.x;
        const Decimal y = yOf(i) - offset.y;
        potentials[h](yIndexOf(i), xIndexOf(i)) += factor.x * x*x + factor.y * y*y;
    }
    potentialPrecalc = potentials.sum() * Complex(0, 1);
    return *this;
}

QuantumSimulation& QuantumSimulation::barrierPotential(const V2 pos, const int width, const List<V2>& slits, const Decimal value) {
    // TODO outsource to Potential class
    const size_t px = toX(pos.x);
    const size_t py = toY(pos.y);
    List<Vec2<size_t>> slitIndices;
    for (const auto& s : slits) {
        if (std::isnan(pos.x))
            slitIndices.push_back(Vec2(toX(s.x), toX(s.y)));
        else
            slitIndices.push_back(Vec2(toY(s.x), toY(s.y)));
    }
    const size_t h = potentials.append(RealMatrix::Zero(H, W));
    // horizontal barrier
    if (std::isnan(pos.x)) {
        for (int i = 0; i < W; ++i) {
            bool isSlit = false;
            for (const auto& s : slitIndices) {
                if (i >= s.x && i < s.y) isSlit = true;
            }
            if (isSlit) continue;
            for (int j = 0; j < width; ++j) {
                potentials[h]((py - width/2 + j), i) = value;
            }
        }
    }
    // vertical barrier
    else {
        for (int i = 0; i < H; ++i) {
            bool isSlit = false;
            for (const auto& s : slitIndices) {
                if (i > s.x && i <= s.y) isSlit = true;
            }
            if (isSlit) continue;
            for (int j = 0; j < width; ++j) {
                potentials[h](i, (px - width/2 + j)) = value;
            }
        }
    }
    potentialPrecalc = potentials.sum() * Complex(0, 1);
    return *this;
}

QuantumSimulation& QuantumSimulation::gaussianDistribution(const V2 offset, const V2 size, const V2 impulse) {
    const auto pPsi = getPsiToChange();
    const V2 sz = size / 2.f;
    for (int i = 0; i < W * H; ++i) {
        constexpr Decimal pi2 = juce::MathConstants<Decimal>::twoPi;
        const Decimal x = xOf(i) - offset.x;
        const Decimal y = yOf(i) - offset.y;
        (*pPsi)(yIndexOf(i), xIndexOf(i)) +=
            std::exp(-Complex(0, 1) * pi2 * impulse.x * x) *
            std::exp(-Complex(0, 1) * pi2 * impulse.y * y) *
            std::exp(-( x*x / (sz.x * sz.x) + y*y / (sz.y * sz.y) ));
    }
    return *this;
}

const ComplexMatrix& QuantumSimulation::getNextFrame(const Decimal timestep, const ModulationData& modulationData) {
    if (!started) {
        started = true;
        psi = initialPsi;
    }

    calculateNextPsi(timestep);
    return psi;
}

void QuantumSimulation::reset() {
    started = false;
}

void QuantumSimulation::calculateNextPsi(const Decimal timestep) {
    static const pocketfft::stride_t stride{ static_cast<long>(H * sizeof(Complex)), sizeof(Complex) };
    static const pocketfft::shape_t shape{ W, H };

    // potential part
    psi *= Eigen::exp(potentialPrecalc * timestep);

    // FFT (to impulse domain)
    pocketfft::c2c(shape, stride, stride, { 0, 1 },
    true, psi.data(), psiFFT.data(), static_cast<Decimal>(1.0 / std::sqrt(w*h)));

    // kinetic part
    psiFFT *= Eigen::exp(thetaPrecalc * timestep);

    // inverse FFT (to spatial domain)
    pocketfft::c2c(shape, stride, stride, { 0, 1 },
    false, psiFFT.data(), psi.data(), static_cast<Decimal>(1.0 / std::sqrt(w*h)));
}
