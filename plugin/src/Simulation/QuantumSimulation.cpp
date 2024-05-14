#include <cmath>

#include "juce_audio_processors/juce_audio_processors.h"
#include "QSynthi2/Simulation/QuantumSimulation.h"
#include "pocketfft_hdronly.h"

QuantumSimulation::QuantumSimulation(const int width, const int height)
    : Simulation()
    , W(width)
    , H(height)
    , w(static_cast<num>(width))
    , h(static_cast<num>(height)) {
    initialPsi = CList(W * H);
    psi = CList(W * H);
    psiFFT = CList(W * H);
    started = false;
}

QuantumSimulation::~QuantumSimulation() = default;

QuantumSimulation& QuantumSimulation::addPotential(const Potential p) {
    // potentials.push_back(p);
    return *this;
}

QuantumSimulation& QuantumSimulation::parabolaPotential(const V2 offset, const V2 factor) {
    const size_t h = potentials.append(RList(W * H));
    for (int i = 0; i < W * H; ++i) {
        const num x = xOf(i) - offset.x;
        const num y = yOf(i) - offset.y;
        potentials[h][i] += factor.x * x*x + factor.y * y*y;
    }
    return *this;
}

QuantumSimulation& QuantumSimulation::barrierPotential(const V2 pos, const int width, const List<V2>& slits, const num value) {
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
    const size_t h = potentials.append(RList(W * H));
    // horizontal barrier
    if (std::isnan(pos.x)) {
        for (int i = 0; i < W; ++i) {
            bool isSlit = false;
            for (const auto& s : slitIndices) {
                if (i >= s.x && i < s.y) isSlit = true;
            }
            if (isSlit) continue;
            for (int j = 0; j < width; ++j) {
                potentials[h][i * H + (py - width/2 + j)] = value;
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
                potentials[h][(px - width/2 + j) * H + i] = value;
            }
        }
    }
    return *this;
}

QuantumSimulation& QuantumSimulation::gaussianDistribution(const V2 offset, const V2 size, const V2 impulse) {
    const auto psi = getPsiToChange();
    const V2 sz = size / 2.f;
    for (int i = 0; i < W * H; ++i) {
        constexpr num pi2 = juce::MathConstants<num>::twoPi;
        const num x = xOf(i) - offset.x;
        const num y = yOf(i) - offset.y;
        (*psi)[i] +=
            std::exp(-cnum(0, 1) * pi2 * impulse.x * x) *
            std::exp(-cnum(0, 1) * pi2 * impulse.y * y) *
            std::exp(-( x*x / (sz.x * sz.x) + y*y / (sz.y * sz.y) ));
    }
    return *this;
}

const CList& QuantumSimulation::getNextFrame(num timestep, ModulationData modulationData) {
    if (!started) {
        started = true;
        psi = initialPsi;
    }

    calculateNextPsi(timestep);
    return psi;
}

void QuantumSimulation::reset() {
    psi = initialPsi;
}

void QuantumSimulation::calculateNextPsi(const num timestep) {
    constexpr num pi2 = juce::MathConstants<num>::twoPi;
    const pocketfft::stride_t stride{ static_cast<long int>(H * sizeof(cnum)), sizeof(cnum) };

    // potential part
    for (size_t i = 0; i < W * H; ++i) {
        num V = 0;
        for (auto& potential : potentials) {
            V += potential[i];
        }
        psi[i] *= std::exp(cnum(0, 1) * timestep * V);
    }

    pocketfft::c2c({ W, H }, stride, stride, { 0, 1 },
        true, psi.data(), psiFFT.data(), static_cast<num>(1.0 / std::sqrt(w*h)));

    // kinetic part
    // TODO outsource window calculation
    for (int i = 0; i < W; ++i) {
        for (int j = 0; j < H; ++j) {
            const num k = pi2 * std::min(static_cast<float>(i), w-static_cast<float>(i)) / w;
            const num l = pi2 * std::min(static_cast<float>(j), h-static_cast<float>(j)) / h;
            const num theta = (k*k + l*l) * timestep;
            psiFFT[i*H+j] *= std::exp(cnum(0, 1) * theta);
        }
    }

    pocketfft::c2c({ W, H }, stride, stride, { 0, 1 },
        false, psiFFT.data(), psi.data(), static_cast<num>(1.0 / std::sqrt(w*h)));
}
