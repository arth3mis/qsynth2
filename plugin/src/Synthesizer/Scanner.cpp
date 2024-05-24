#include "QSynthi2/Synthesizer/Scanner.h"
#include "QSynthi2/Data.h"

extern Data sharedData;


Scanner::Scanner() {

}



Eigen::ArrayXX<Decimal> Scanner::getValuesAt(const Eigen::ArrayXX<Decimal> &position0to1, const ModulationData& modulationData) {
    return (position0to1 * juce::MathConstants<Decimal>::twoPi).sin();
}


void Scanner::prepareToPlay(Decimal newSampleRate) {
    this->sampleRate = newSampleRate;
}



void Scanner::nextSample() {
}




void Scanner::restart() {
}
