#include "QSynthi2/Synthesizer/Scanner.h"
#include "QSynthi2/Data.h"

extern Data sharedData;


Scanner::Scanner() = default;



Eigen::ArrayXX<Decimal> Scanner::getValuesAt(const Eigen::ArrayXX<Decimal> &position0to1, const ModulationData& modulationData) {

    // TODO: Add support for 2d positions
    // auto frameBufferTimestamps = sharedData.frameBufferTimestamps.replicate(1, position0to1.cols());

    auto lineOfInterestX = sharedData.parameters->lineOfInterestX->getModulated(modulationData);
    auto lineOfInterestY = sharedData.parameters->lineOfInterestY->getModulated(modulationData);
    auto lineOfInterestRotation = sharedData.parameters->lineOfInterestRotation->getModulated(modulationData);

    auto timestamps = sharedData.frameBufferTimestamps;





    auto sinus = (position0to1 * juce::MathConstants<Decimal>::twoPi).sin();
    auto rectangle = 0.25 * sinus.sign();
    auto t = sharedData.parameters->timbre->getModulated(modulationData).square();

    return (1-t) * sinus + t * rectangle;
}


void Scanner::prepareToPlay(Decimal newSampleRate) {
    this->sampleRate = newSampleRate;
}



void Scanner::restart() {
}
