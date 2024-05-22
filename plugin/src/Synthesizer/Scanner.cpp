#include "QSynthi2/Synthesizer/Scanner.h"
#include "QSynthi2/Data.h"

extern Data sharedData;


Scanner::Scanner() {
    // constexpr int SIM_SIZE = 128;
    // sim = std::make_unique<QuantumSimulation>(QuantumSimulation(SIM_SIZE,SIM_SIZE)
    //     .barrierPotential({-0.0, NAN}, 2, {{-0.2, -0.1}, {0.1, 0.2}}, 1e30)
    //     .parabolaPotential({0, 0}, {2, 1.5})
    //     .gaussianDistribution({-0.4, 0}, {0.25, 0.25}, {4, 0}));
    // sharedData.simWidth = SIM_SIZE;
    // sharedData.simHeight = SIM_SIZE;
    // timestep = 0.2;

}

Decimal Scanner::getValueAt(Decimal at, const ModulationData& modulationData) {
    /*
    num sinus = sin(juce::MathConstants<num>::twoPi * at);
    num rectangle = std::copysign (1.0, sinus);
    num timbre = sharedData.parameters.timbre->getModulated(modulationData);

    return (1 - timbre) * sinus + timbre * rectangle;
    */
    // TODO

    Decimal x = 128 * at;
    long y = 127 - (size_t)(128 * sharedData.parameters.timbre->getModulated(modulationData));
    Decimal t = fmod(x, (Decimal)1);

    long floorX = floor(x);
    long ceilX = std::min(127.0, ceil(x));

    // juce::Logger::writeToLog(juce::String(std::abs(frame(0,0))));

    sharedData.scanlineY = y;
    auto frame = sharedData.getSimulationScanFrame();

    return (1-t) * std::abs(frame->coeff(y, floorX)) + t * std::abs(frame->coeff(y, ceilX));

}

void Scanner::prepareToPlay(Decimal newSampleRate) {
    this->sampleRate = newSampleRate;
}

void Scanner::nextSample() {
}


void Scanner::restart() {
}
