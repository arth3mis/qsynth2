#include <cmath>

#include "QSynthi2/Simulation/QuantumSimulation.h"
#include "pocketfft_hdronly.h"

QuantumSimulation::QuantumSimulation(const int width, const int height, const num timestep)
    : Simulation()
    , W(width)
    , H(height)
    , w(static_cast<num>(width))
    , h(static_cast<num>(height)) {
    dt = timestep;
    psi = CList(W * H);
    potential = list2D(W, H, cnum());
}

QuantumSimulation::~QuantumSimulation() = default;

QuantumSimulation& QuantumSimulation::parabolaPotential(const V2 offset, const V2 factor) {
    for (int i = 0; i < W; ++i) {
        for (int j = 0; j < H; ++j) {
            const num x = static_cast<num>(i) - w/2.f + offset.x * w;
            const num y = static_cast<num>(j) - h/2.f + offset.y * h;
            potential[i][j] += factor.x * x * x + factor.y * y * y;
        }
    }
    return *this;
}

QuantumSimulation& QuantumSimulation::gaussianDistribution(const V2 offset, const V2 relSize, V2 impulse) {
    for (int i = 0; i < W * H; ++i) {
        const num x = xOf(i) - offset.x;
        const num y = yOf(i) - offset.y;
        psi[i] +=
            std::exp(-( x*x / (relSize.x * relSize.x) + y*y / (relSize.y * relSize.y) ));
    }
    return *this;
}

CList& QuantumSimulation::getNextFrame() {
    calculateNextPsi();
    return psi;
}

void QuantumSimulation::calculateNextPsi() {
    auto out = CList(W * H);
    const pocketfft::stride_t stride{ static_cast<long int>(H * sizeof(cnum)), sizeof(cnum) };

    pocketfft::c2c({ W, H }, stride, stride, { 0, 1 },
        true, psi.data(), out.data(), static_cast<num>(1.0 / std::sqrt(w*h)));

    pocketfft::c2c({ W, H }, stride, stride, { 0, 1 },
        false, out.data(), psi.data(), static_cast<num>(1.0 / std::sqrt(w*h)));



}
