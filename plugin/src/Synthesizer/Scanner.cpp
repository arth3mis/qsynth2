#include "QSynthi2/Synthesizer/Scanner.h"



num Scanner::getValueAt(num at, const ModulationData& modulationData) {
    // TODO
    return tanh(sin(juce::MathConstants<num>::twoPi * at));
}

void Scanner::prepareToPlay(num sampleRate) {
    // TODO getSimulationSpeed
}

void Scanner::nextSample() {
    // TODO
}
