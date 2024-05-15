#include "QSynthi2/Synthesizer/Scanner.h"
#include "QSynthi2/Data.h"

extern Data sharedData;


Scanner::Scanner() {
    constexpr int SIM_SIZE = 128;
    sim = std::make_unique<QuantumSimulation>(QuantumSimulation(SIM_SIZE,SIM_SIZE)
        .barrierPotential({-0.0, NAN}, 2, {{-0.2, -0.1}, {0.1, 0.2}}, 1e30)
        .parabolaPotential({0, 0}, {2, 1.5})
        .gaussianDistribution({-0.4, 0}, {0.25, 0.25}, {4, 0}));

    sharedData.simWidth = SIM_SIZE;
    sharedData.simHeight = SIM_SIZE;

    timestep = 0.5;
}

num Scanner::getValueAt(num at, const ModulationData& modulationData) {
    num sinus = sin(juce::MathConstants<num>::twoPi * at);
    num rectangle = std::copysign (1.0, sinus);
    num timbre = sharedData.parameters.timbre->getModulated(modulationData);

    // TODO
    return (1 - timbre) * sinus + timbre * rectangle;
}

void Scanner::prepareToPlay(num sampleRate) {
    // TODO getSimulationSpeed
}

void Scanner::nextSample() {
    // TODO correct implementation
    time++;
    if (time % 441 != 0) {
        return;
    }

    simFramePrev = simFrameCurrent;
    simFrameCurrent = sim->getNextFrame(timestep, {});

    sharedData.setSimulationDisplayFrame(simFrameCurrent.map<num>([](const cnum c){ return std::abs(c); }));
}

void Scanner::restart() {
    sim->reset();
}
