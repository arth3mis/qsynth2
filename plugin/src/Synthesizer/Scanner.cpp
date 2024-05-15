#include "QSynthi2/Synthesizer/Scanner.h"
#include "QSynthi2/Data.h"

extern Data sharedData;


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
    // TODO
}
