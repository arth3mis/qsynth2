#include <cmath>

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
    psiP = CList(W * H);
    started = false;
}

QuantumSimulation::~QuantumSimulation() = default;

QuantumSimulation& QuantumSimulation::potential(const Potential p) {
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

QuantumSimulation& QuantumSimulation::barrierPotential(const V2 start, const V2 end, int width, num value) {
    // const int px = static_cast<int>(pos.x * w/2);
    // const int py = static_cast<int>(pos.y * h/2);
    const size_t h = potentials.append(RList(W * H));
    for (int i = 0; i < W * H; ++i) {
        // if (xOf(i) == pos.x);
    }
    return *this;
}

QuantumSimulation& QuantumSimulation::gaussianDistribution(const V2 offset, const V2 size, const V2 impulse) {
    for (int i = 0; i < W * H; ++i) {
        constexpr num pi2 = M_PI * 2;
        const num x = xOf(i) - offset.x;
        const num y = yOf(i) - offset.y;
        initialPsi[i] +=
            std::exp(-cnum(0, 1) * pi2 * impulse.x * x) *
            std::exp(-cnum(0, 1) * pi2 * impulse.y * y) *
            std::exp(-( x*x / (size.x * size.x) + y*y / (size.y * size.y) ));
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
    constexpr num pi2 = M_PI * 2;
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
        true, psi.data(), psiP.data(), static_cast<num>(1.0 / std::sqrt(w*h)));

    // kinetic part
    // TODO outsource window calculation
    for (int i = 0; i < W; ++i) {
        for (int j = 0; j < H; ++j) {
            const num k = pi2 * std::min(static_cast<float>(i), w-static_cast<float>(i)) / w;
            const num l = pi2 * std::min(static_cast<float>(j), h-static_cast<float>(j)) / h;
            const num theta = (k*k + l*l) * timestep;
            psiP[i*H+j] *= std::exp(cnum(0, 1) * theta);
        }
    }

    pocketfft::c2c({ W, H }, stride, stride, { 0, 1 },
        false, psiP.data(), psi.data(), static_cast<num>(1.0 / std::sqrt(w*h)));
}
