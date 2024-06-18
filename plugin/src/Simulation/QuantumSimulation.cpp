#include "QSynthi2/Simulation/QuantumSimulation.h"
#include "QSynthi2/Juce.h"
#include "QSynthi2/FFT.h"
#include <cmath>
#include <QSynthi2/Data.h>

extern Data sharedData;

QuantumSimulation::QuantumSimulation(const int width, const int height)
    : Simulation()
    , W(width)
    , H(height)
    , w(static_cast<Decimal>(width))
    , h(static_cast<Decimal>(height)) {
    barrierPotentialTemp = RealMatrix::Zero(H, W);
    parabolaPotentialTemp = RealMatrix::Zero(H, W);

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
    parabolaPotentialTemp = RealMatrix::Zero(H, W);
    for (int i = 0; i < W * H; ++i) {
        const Decimal x = xOf(i) - offset.x;
        const Decimal y = yOf(i) - offset.y;
        parabolaPotentialTemp(yIndexOf(i), xIndexOf(i)) += factor.x * x*x + factor.y * y*y;
    }
    potentialPrecalc = (parabolaPotentialTemp + barrierPotentialTemp) * Complex(0, 1);
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
    barrierPotentialTemp = RealMatrix::Zero(H, W);
    // horizontal barrier
    if (std::isnan(pos.x)) {
        for (int i = 0; i < W; ++i) {
            bool isSlit = false;
            for (const auto& s : slitIndices) {
                if (i >= s.x && i < s.y) isSlit = true;
            }
            if (isSlit) continue;
            for (int j = 0; j < width; ++j) {
                barrierPotentialTemp((py - width/2 + j), i) = value;
            }
        }
    }
    // vertical barrier
    else if (std::isnan(pos.y)) {
        for (int i = 0; i < H; ++i) {
            bool isSlit = false;
            for (const auto& s : slitIndices) {
                if (i > s.x && i <= s.y) isSlit = true;
            }
            if (isSlit) continue;
            for (int j = 0; j < width; ++j) {
                barrierPotentialTemp(i, (px - width/2 + j)) = value;
            }
        }
    }
    potentialPrecalc = (parabolaPotentialTemp + barrierPotentialTemp) * Complex(0, 1);
    return *this;
}

void QuantumSimulation::resetGaussianDistribution(const bool onlyApplyToInitialPsi) {
    const auto pPsi = onlyApplyToInitialPsi ? &initialPsi : getPsiToChange();
    *pPsi = ComplexMatrix::Zero(H, W);
}

QuantumSimulation& QuantumSimulation::gaussianDistribution(const V2 offset, const V2 size, const V2 impulse, const bool onlyApplyToInitialPsi) {
    const auto pPsi = onlyApplyToInitialPsi ? &initialPsi : getPsiToChange();
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

SimulationFramePointer QuantumSimulation::getNextFrame(const Decimal timestep, const ModulationData& modulationData) {
    if (!started) {
        started = true;
        psi = initialPsi;
    }

    calculateNextPsi(timestep);
    return std::make_shared<QuantumSimulationFrame>(psi);
}

void QuantumSimulation::reset() {
    started = false;
}

void QuantumSimulation::updateParameters(const ParameterCollection *p, const List<ModulationData*> &m) {
    const Decimal l_gaussianOffsetX = p->gaussianOffsetX->getSingleModulated(m), l_gaussianOffsetY = p->gaussianOffsetY->getSingleModulated(m);
    const Decimal l_gaussianStretchX = p->gaussianStretchX->getSingleModulated(m), l_gaussianStretchY = p->gaussianStretchY->getSingleModulated(m);
    const Decimal l_gaussianImpulseX = p->gaussianImpulseX->getSingleModulated(m), l_gaussianImpulseY = p->gaussianImpulseY->getSingleModulated(m);
    const Decimal l_parabolaOffsetX = p->parabolaOffsetX->getSingleModulated(m), l_parabolaOffsetY = p->parabolaOffsetY->getSingleModulated(m);
    const Decimal l_parabolaFactorX = p->parabolaFactorX->getSingleModulated(m), l_parabolaFactorY = p->parabolaFactorY->getSingleModulated(m);
    const Decimal l_barrierOffsetX = p->barrierOffsetX->getSingleModulated(m);
    const Decimal l_barrierWidth = p->barrierWidth->getSingleModulated(m);
    const Decimal l_barrierSlit1Start = p->barrierSlit1Start->getSingleModulated(m), l_barrierSlit1End = p->barrierSlit1End->getSingleModulated(m);
    const Decimal l_barrierSlit2Start = p->barrierSlit2Start->getSingleModulated(m), l_barrierSlit2End = p->barrierSlit2End->getSingleModulated(m);

    if (!juce::approximatelyEqual(gaussianOffsetX, l_gaussianOffsetX) ||
        !juce::approximatelyEqual(gaussianOffsetY, l_gaussianOffsetY) ||
        !juce::approximatelyEqual(gaussianStretchX, l_gaussianStretchX) ||
        !juce::approximatelyEqual(gaussianStretchY, l_gaussianStretchY) ||
        !juce::approximatelyEqual(gaussianImpulseX, l_gaussianImpulseX) ||
        !juce::approximatelyEqual(gaussianImpulseY, l_gaussianImpulseY)) {
        resetGaussianDistribution(true);
        gaussianDistribution({l_gaussianOffsetX, l_gaussianOffsetY}, {l_gaussianStretchX, l_gaussianStretchY}, {l_gaussianImpulseX, l_gaussianImpulseY}, true);
    }
    if (!juce::approximatelyEqual(parabolaOffsetX, l_parabolaOffsetX) ||
        !juce::approximatelyEqual(parabolaOffsetY, l_parabolaOffsetY) ||
        !juce::approximatelyEqual(parabolaFactorX, l_parabolaFactorX) ||
        !juce::approximatelyEqual(parabolaFactorY, l_parabolaFactorY)) {
        parabolaPotential({l_parabolaOffsetX, l_parabolaOffsetY}, {l_parabolaFactorX, l_parabolaFactorY});
    }
    if (!juce::approximatelyEqual(barrierOffsetX, l_barrierOffsetX) ||
        !juce::approximatelyEqual(barrierWidth, l_barrierWidth) ||
        !juce::approximatelyEqual(barrierSlit1Start, l_barrierSlit1Start) ||
        !juce::approximatelyEqual(barrierSlit1End, l_barrierSlit1End) ||
        !juce::approximatelyEqual(barrierSlit2Start, l_barrierSlit2Start) ||
        !juce::approximatelyEqual(barrierSlit2End, l_barrierSlit2End)) {
        List<V2> slits;
        if (l_barrierSlit1Start >= -1 && -l_barrierSlit1End >= -1)
            slits.append({l_barrierSlit1Start, l_barrierSlit1End});
        if (l_barrierSlit2Start >= -1 && -l_barrierSlit2End >= -1)
            slits.append({l_barrierSlit2Start, l_barrierSlit2End});
        barrierPotential({l_barrierOffsetX < -1 ? NAN : l_barrierOffsetX, NAN}, static_cast<int>(std::round(l_barrierWidth)), slits, 1e30);
        sharedData.barrierX = l_barrierOffsetX;
        sharedData.barrierWidth = l_barrierWidth;
        sharedData.barrierSlits = slits;
    }

    gaussianOffsetX = l_gaussianOffsetX;
    gaussianOffsetY = l_gaussianOffsetY;
    gaussianStretchX = l_gaussianStretchX;
    gaussianStretchY = l_gaussianStretchY;
    gaussianImpulseX = l_gaussianImpulseX;
    gaussianImpulseY = l_gaussianImpulseY;
    parabolaOffsetX = l_parabolaOffsetX;
    parabolaOffsetY = l_parabolaOffsetY;
    parabolaFactorX = l_parabolaFactorX;
    parabolaFactorY = l_parabolaFactorY;
    barrierOffsetX = l_barrierOffsetX;
    barrierWidth = l_barrierWidth;
    barrierSlit1Start = l_barrierSlit1Start;
    barrierSlit1End = l_barrierSlit1End;
    barrierSlit2Start = l_barrierSlit2Start;
    barrierSlit2End = l_barrierSlit2End;
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
