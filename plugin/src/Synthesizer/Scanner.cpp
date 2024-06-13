#include <utility>

#include "QSynthi2/Synthesizer/Scanner.h"
#include "QSynthi2/Data.h"

extern Data sharedData;


Scanner::Scanner(std::shared_ptr<VoiceData> voiceData) : voiceData(std::move(voiceData)) {

}



static Eigen::ArrayX<Decimal> clipDivisor(const Eigen::ArrayX<Decimal> &x, const Eigen::ArrayX<Decimal> &y, const Eigen::ArrayX<Decimal> &anchorX, const Eigen::ArrayX<Decimal> &anchorY) {
    // TODO: optimize anchor divisions?
    return  (1 / ( 1 - anchorX) * x).cwiseMax(
            (1 / (-1 - anchorX) * x)).cwiseMax(
            (1 / ( 1 - anchorY) * y)).cwiseMax(
            (1 / (-1 - anchorY) * y)).cwiseMax(
            1);
}



Eigen::ArrayXX<Decimal> Scanner::getValuesAt(const Eigen::ArrayXX<Decimal> &position0to1, const ModulationData& modulationData) {

    // TODO: Add support for 2d positions
    // auto frameBufferTimestamps = sharedData.frameBufferTimestamps.replicate(1, position0to1.cols());

    Eigen::ArrayX<Decimal> lineOfInterestX = sharedData.parameters->lineOfInterestX->getModulated(modulationData).cwiseMin(1 - 1e-9).cwiseMax(-1 + 1e-9);
    Eigen::ArrayX<Decimal> lineOfInterestY = -sharedData.parameters->lineOfInterestY->getModulated(modulationData).cwiseMin(1 - 1e-9).cwiseMax(-1 + 1e-9);
    Eigen::ArrayX<Decimal> lineOfInterestLength = sharedData.parameters->lineOfInterestLength->getModulated(modulationData);
    Eigen::ArrayX<Decimal> lineOfInterestRotation = sharedData.parameters->lineOfInterestRotation->getModulated(modulationData);

    // TODO: Broadcast all to 2d
    auto timestamps = sharedData.frameBufferTimestamps;

    Eigen::ArrayX<Decimal> xEnd = lineOfInterestLength * lineOfInterestRotation.cos();
    Eigen::ArrayX<Decimal> yEnd = lineOfInterestLength * lineOfInterestRotation.sin();

    Eigen::ArrayX<Decimal> xStart = -xEnd;
    Eigen::ArrayX<Decimal> yStart = -yEnd;

    auto endClipDivisor = clipDivisor(xEnd, yEnd, lineOfInterestX, lineOfInterestY);
    auto startClipDivisor = clipDivisor(xStart, yStart, lineOfInterestX, lineOfInterestY);

    xEnd = xEnd / endClipDivisor + lineOfInterestX;
    yEnd = yEnd / endClipDivisor + lineOfInterestY;
    xStart = xStart / startClipDivisor + lineOfInterestX;
    yStart = yStart / startClipDivisor + lineOfInterestY;

    voiceData->scanlineEndX = xEnd(0);
    voiceData->scanlineStartX = xStart(0);
    voiceData->scanlineEndY = yEnd(0);
    voiceData->scanlineStartY = yStart(0);

    auto x = (1-position0to1) * xStart + position0to1 * xEnd;
    auto y = (1-position0to1) * yStart + position0to1 * yEnd;

    Eigen::ArrayXX<Decimal> xScaled = (x + 1) / 2 * sharedData.simulationWidth;
    Eigen::ArrayXX<Decimal> yScaled = (y + 1) / 2 * sharedData.simulationHeight;

    return getInterpolated(sharedData.frameBuffer, timestamps, yScaled, xScaled);

}



Eigen::ArrayXX<Decimal> Scanner::getInterpolated(const FrameList &frameBuffer,
                                                 const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                 const Eigen::ArrayXX<Decimal> &y,
                                                 const Eigen::ArrayXX<Decimal> &x) {

    jassert(frameBufferTimestamps.size() == y.size() && y.size() == x.size()); // All Array sizes must match

    Eigen::ArrayXX<Complex> interpolatedValues = Eigen::ArrayXX<Complex>(x.rows(), x.cols());

    for (Eigen::Index i = 0; i < x.size(); i++) {
        auto timestampsFloor = static_cast<size_t>(floor(frameBufferTimestamps(i)));
        auto timestampsCeil  = static_cast<size_t>(ceil (frameBufferTimestamps(i)));
        Decimal timestampsT = fmod(frameBufferTimestamps(i), 1);

        auto yFloor = static_cast<Eigen::Index>(floor(y(i)));
        auto yCeil  = static_cast<Eigen::Index>(static_cast<unsigned long>(ceil (y(i))) % sharedData.simulationHeight);
        Decimal yT = fmod(y(i), 1);

        auto xFloor = static_cast<Eigen::Index>(floor(x(i)));
        auto xCeil  = static_cast<Eigen::Index>(static_cast<unsigned long>(ceil (x(i))) % sharedData.simulationWidth);
        Decimal xT = fmod(x(i), 1);

        jassert(timestampsFloor >= 0 && timestampsFloor < frameBuffer.size());
        jassert(timestampsCeil  >= 0 && timestampsCeil  < frameBuffer.size());
        jassert(yFloor >= 0 && yFloor < frameBuffer.at(timestampsFloor)->cols());
        jassert(yCeil  >= 0 && yCeil  < frameBuffer.at(timestampsFloor)->cols());
        jassert(xFloor >= 0 && xFloor < frameBuffer.at(timestampsFloor)->rows());
        jassert(xCeil  >= 0 && xCeil  < frameBuffer.at(timestampsFloor)->rows());

        interpolatedValues(i) = (1-timestampsT) * ((1-yT) * ((1-xT) * frameBuffer.at(timestampsFloor)->toDecimal(yFloor, xFloor)
                                                          +     xT  * frameBuffer.at(timestampsFloor)->toDecimal(yFloor, xCeil ))
                                                +     yT  * ((1-xT) * frameBuffer.at(timestampsFloor)->toDecimal(yCeil,  xFloor)
                                                          +     xT  * frameBuffer.at(timestampsFloor)->toDecimal(yCeil,  xCeil )))
                              +    timestampsT  * ((1-yT) * ((1-xT) * frameBuffer.at(timestampsCeil )->toDecimal(yFloor, xFloor)
                                                          +     xT  * frameBuffer.at(timestampsCeil )->toDecimal(yFloor, xCeil ))
                                                +     yT  * ((1-xT) * frameBuffer.at(timestampsCeil )->toDecimal(yCeil,  xFloor)
                                                          +     xT  * frameBuffer.at(timestampsCeil )->toDecimal(yCeil,  xCeil )));
    }

    return toDecimal(interpolatedValues);
}




Eigen::ArrayXX<Decimal> Scanner::toDecimal(const Eigen::ArrayXX<Complex> &simulationValues) {
    // TODO: Parameters
    return simulationValues.abs();
}



void Scanner::prepareToPlay(Decimal newSampleRate) {
    this->sampleRate = newSampleRate;
}



void Scanner::restart() {
}
