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



Eigen::ArrayXX<Decimal> Scanner::getValuesAt(const Eigen::ArrayXX<Decimal> &position0to1, std::function<Eigen::ArrayXX<Decimal>(const FrameList &frameBuffer,
                                                                                                                                const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                                                                                                const Eigen::ArrayXX<Decimal> &y,
                                                                                                                                const Eigen::ArrayXX<Decimal> &x)> interpolation, const ModulationData& modulationData) {

    // TODO: Add support for 2d positions
    // auto frameBufferTimestamps = sharedData.frameBufferTimestamps.replicate(1, position0to1.cols());

    Eigen::ArrayX<Decimal> lineOfInterestX = sharedData.parameters->lineOfInterestX->getModulated(modulationData).cwiseMin(1 - 1e-9).cwiseMax(-1 + 1e-9);
    Eigen::ArrayX<Decimal> lineOfInterestY = -sharedData.parameters->lineOfInterestY->getModulated(modulationData).cwiseMin(1 - 1e-9).cwiseMax(-1 + 1e-9);
    Eigen::ArrayX<Decimal> lineOfInterestLength = sharedData.parameters->lineOfInterestLength->getModulated(modulationData);
    Eigen::ArrayX<Decimal> lineOfInterestRotation = sharedData.parameters->lineOfInterestRotation->getModulated(modulationData);

    // TODO: Broadcast all to 2d
    auto timestamps = sharedData.frameBufferTimestamps.replicate(1, position0to1.cols());

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

    auto x = (1-position0to1).colwise() * xStart + position0to1.colwise() * xEnd;
    auto y = (1-position0to1).colwise() * yStart + position0to1.colwise() * yEnd;

    Eigen::ArrayXX<Decimal> xScaled = (x + 1) / 2 * sharedData.simulationWidth;
    Eigen::ArrayXX<Decimal> yScaled = (y + 1) / 2 * sharedData.simulationHeight;

    return interpolation(sharedData.frameBuffer, timestamps, yScaled, xScaled);

}



Eigen::ArrayXX<Decimal>
Scanner::noInterpolation(const FrameList &frameBuffer, const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                         const Eigen::ArrayXX<Decimal> &y, const Eigen::ArrayXX<Decimal> &x) {
    jassert(frameBufferTimestamps.size() == y.size() && y.size() == x.size()); // All Array sizes must match

    auto interpolatedValues = RealMatrix(x.rows(), x.cols());

    for (Eigen::Index i = 0; i < x.size(); i++) {
        interpolatedValues(i) = frameBuffer.at(static_cast<size_t>(frameBufferTimestamps(i)))->toDecimal(static_cast<Eigen::Index>(y(i)), static_cast<Eigen::Index>(x(i)));
    }

    return interpolatedValues;
}


Eigen::ArrayXX<Decimal>
Scanner::linearInterpolation(const FrameList &frameBuffer, const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                             const Eigen::ArrayXX<Decimal> &y, const Eigen::ArrayXX<Decimal> &x) {
    auto interpolatedValues = RealMatrix(x.rows(), x.cols());

    for (Eigen::Index i = 0; i < x.size(); i++) {
        auto timestampsFloor = static_cast<size_t>(floor(frameBufferTimestamps(i)));
        auto timestampsCeil  = static_cast<size_t>(ceil (frameBufferTimestamps(i)));
        Decimal timestampsT = fmod(frameBufferTimestamps(i), 1);

        auto yFloor = static_cast<Eigen::Index>(static_cast<unsigned long>(floor(y(i))) % sharedData.simulationHeight);
        auto yCeil  = static_cast<Eigen::Index>(static_cast<unsigned long>(ceil (y(i))) % sharedData.simulationHeight);
        Decimal yT = fmod(y(i), 1);

        auto xFloor = static_cast<Eigen::Index>(static_cast<unsigned long>(floor(x(i))) % sharedData.simulationWidth);
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

    return interpolatedValues;
}



Eigen::ArrayXX<Decimal> Scanner::bicubicInterpolation(const FrameList &frameBuffer,
                                                      const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                      const Eigen::ArrayXX<Decimal> &y,
                                                      const Eigen::ArrayXX<Decimal> &x) {

    jassert(frameBufferTimestamps.size() == y.size() && y.size() == x.size()); // All Array sizes must match

    auto interpolatedValues = RealMatrix(x.rows(), x.cols());

    for (Eigen::Index i = 0; i < x.size(); i++) {
        interpolatedValues(i) = getBicubicInterpolated(frameBuffer, frameBufferTimestamps(i), y(i), x(i));
    }

    return interpolatedValues;
}



Decimal Scanner::getBicubicInterpolated(const FrameList &frameBuffer, Decimal frameBufferTimestamp, Decimal y, Decimal x) {
    auto timestampFloor = static_cast<size_t>(floor(frameBufferTimestamp));
    auto timestampCeil  = static_cast<size_t>(ceil (frameBufferTimestamp));
    Decimal t = fmod(frameBufferTimestamp, 1);

    return (1-t) * getBicubicInterpolated(frameBuffer, timestampFloor, y, x) +
              t  * getBicubicInterpolated(frameBuffer, timestampCeil,  y, x);
}



Decimal Scanner::getBicubicInterpolated(const FrameList &frameBuffer, size_t frameBufferTimestamp, Decimal y, Decimal x) {
    Decimal t = fmod(y, 1);

    Decimal y0 = getBicubicInterpolated(frameBuffer, frameBufferTimestamp, bicubicIndex<0>(y, sharedData.simulationHeight), x);
    Decimal y1 = getBicubicInterpolated(frameBuffer, frameBufferTimestamp, bicubicIndex<1>(y, sharedData.simulationHeight), x);
    Decimal y2 = getBicubicInterpolated(frameBuffer, frameBufferTimestamp, bicubicIndex<2>(y, sharedData.simulationHeight), x);
    Decimal y3 = getBicubicInterpolated(frameBuffer, frameBufferTimestamp, bicubicIndex<3>(y, sharedData.simulationHeight), x);

    return bicubicCoefficient<0>(y0, y1, y2, y3)
         + bicubicCoefficient<1>(y0, y1, y2, y3) * t
         + bicubicCoefficient<2>(y0, y1, y2, y3) * t*t
         + bicubicCoefficient<3>(y0, y1, y2, y3) * t*t*t;

}



Decimal Scanner::getBicubicInterpolated(const FrameList &frameBuffer, size_t frameBufferTimestamp, Eigen::Index y, Decimal x) {
    Decimal t = fmod(x, 1);

    Decimal x0 = frameBuffer.at(frameBufferTimestamp)->toDecimal(y, bicubicIndex<0>(x, sharedData.simulationWidth));
    Decimal x1 = frameBuffer.at(frameBufferTimestamp)->toDecimal(y, bicubicIndex<1>(x, sharedData.simulationWidth));
    Decimal x2 = frameBuffer.at(frameBufferTimestamp)->toDecimal(y, bicubicIndex<2>(x, sharedData.simulationWidth));
    Decimal x3 = frameBuffer.at(frameBufferTimestamp)->toDecimal(y, bicubicIndex<3>(x, sharedData.simulationWidth));

    return bicubicCoefficient<0>(x0, x1, x2, x3)
         + bicubicCoefficient<1>(x0, x1, x2, x3) * t
         + bicubicCoefficient<2>(x0, x1, x2, x3) * t*t
         + bicubicCoefficient<3>(x0, x1, x2, x3) * t*t*t;

}


template <int pointNumber>
Eigen::Index Scanner::bicubicIndex(Decimal interpolationIndex, size_t size) {
    static_assert(pointNumber >=0 && pointNumber < 4);
    return juce::negativeAwareModulo(static_cast<Eigen::Index>(floor(interpolationIndex) - 1.0 + pointNumber), static_cast<Eigen::Index>(size));
}



template<int pointNumber>
Decimal Scanner::bicubicCoefficient(Decimal v0, Decimal v1, Decimal v2, Decimal v3) {
    static_assert(pointNumber >=0 && pointNumber < 4);

    switch (pointNumber) {
        case(0):
            return v1;
        case(1):
            return -v0/3.0 - v1/2.0 + v2     - v3/6.0;
        case(2):
            return  v0/2.0 - v1     + v2/2.0;
        case(3):
            return -v0/6.0 + v1/2.0 - v2/2.0 + v3/6.0;
    }
}



void Scanner::prepareToPlay(Decimal newSampleRate) {
    this->sampleRate = newSampleRate;
}



void Scanner::restart() {
}
