#include "QSynthi2/Synthesizer/Scanner.h"
#include "QSynthi2/Data.h"

extern Data sharedData;


Scanner::Scanner(const std::shared_ptr<Simulation>& simRef)
    : sim(simRef) {
    // constexpr int SIM_SIZE = 128;
    // sim = std::make_unique<QuantumSimulation>(QuantumSimulation(SIM_SIZE,SIM_SIZE)
    //     .barrierPotential({-0.0, NAN}, 2, {{-0.2, -0.1}, {0.1, 0.2}}, 1e30)
    //     .parabolaPotential({0, 0}, {2, 1.5})
    //     .gaussianDistribution({-0.4, 0}, {0.25, 0.25}, {4, 0}));
    // sharedData.simWidth = SIM_SIZE;
    // sharedData.simHeight = SIM_SIZE;

    timestep = 0.2;
}

num Scanner::getValueAt(num at, const ModulationData& modulationData) {
    num sinus = sin(juce::MathConstants<num>::twoPi * at);
    num rectangle = std::copysign (1.0, sinus);
    num timbre = sharedData.parameters.timbre->getModulated(modulationData);

    // TODO
    return (1 - timbre) * sinus + timbre * rectangle;
}

void Scanner::prepareToPlay(num newSampleRate) {
    this->sampleRate = newSampleRate;
}

void Scanner::nextSample() {
    // TODO correct implementation
    //      only used for testing right now, but the logic needs to move into AJAudioProcessor,
    //      which propagates the simulation and adds to frameBuffer that each Scanner can access values from.
    time++;
    if (time % (size_t)(sampleRate / 80) != 0) {
        return;
    }

    sharedData.functionCallStopwatch.stop();
    sharedData.simulationStopwatch.start();

    simFramePrev = simFrameCurrent;
    simFrameCurrent = sim->getNextFrame(timestep, {});

    sharedData.setSimulationDisplayFrame(simFrameCurrent);//.map<num>([](const cnum c){ return std::abs(c); }));

    sharedData.simulationStopwatch.stop();
    sharedData.functionCallStopwatch.start();
 }


void Scanner::restart() {
    sim->reset();
}
