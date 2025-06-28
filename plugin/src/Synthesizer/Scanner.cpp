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



void Scanner::getValuesAt(const Eigen::ArrayXX<Decimal> &position0to1, InterpolationFunc interpolation, const ModulationData& modulationData, Eigen::ArrayXX<Decimal>& outputBuffer) {

    // auto frameBufferTimestamps = sharedData.frameBufferTimestamps.replicate(1, position0to1.cols());
    int lineOfInterestShape = sharedData.parameters->lineOfInterestShape->getIndex();
    voiceData->lineOfInterestShape = lineOfInterestShape;

    if (lineOfInterestShape == 0) {
        return getValuesLine(position0to1, interpolation, modulationData, outputBuffer);
    } else {
        return getValuesCircle(position0to1, interpolation, modulationData, outputBuffer);
    }
}



void
Scanner::noInterpolation(const FrameList &frameBuffer, const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                         const Eigen::ArrayXX<Decimal> &y, const Eigen::ArrayXX<Decimal> &x, Eigen::ArrayXX<Decimal>& outputBuffer) {
    jassert(frameBufferTimestamps.size() == y.size() && y.size() == x.size()); // All Array sizes must match

    // interpolatedValues = RealMatrix(x.rows(), x.cols());
    if (outputBuffer.rows() < x.rows() || outputBuffer.cols() < x.cols()) {
        outputBuffer.resize(x.rows(), x.cols());
    }

    for (Eigen::Index i = 0; i < x.size(); i++) {
        outputBuffer(i) = frameBuffer.at(static_cast<size_t>(frameBufferTimestamps(i)))->toDecimal(static_cast<Eigen::Index>(y(i)), static_cast<Eigen::Index>(x(i)));
    }
}


void
Scanner::linearInterpolation(const FrameList &frameBuffer, const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                             const Eigen::ArrayXX<Decimal> &y, const Eigen::ArrayXX<Decimal> &x, Eigen::ArrayXX<Decimal>& outputBuffer) {
    // interpolatedValues = RealMatrix(x.rows(), x.cols());
    if (outputBuffer.rows() < x.rows() || outputBuffer.cols() < x.cols()) {
        outputBuffer.resize(x.rows(), x.cols());
        juce::Logger::writeToLog("linear interpolation: buffer resized to " + juce::String(outputBuffer.size()) + ": " + juce::String(outputBuffer.rows()) + "*" + juce::String(outputBuffer.cols()));
    }

    for (Eigen::Index i = 0; i < x.size(); i++) {
        auto timestampsFloor = static_cast<size_t>(floor(frameBufferTimestamps(i)));
        auto timestampsCeil  = static_cast<size_t>(ceil (frameBufferTimestamps(i)));
        Decimal timestampsT = fmod(frameBufferTimestamps(i), 1);

        auto yFloor = static_cast<long>(fmod(floor(y(i)), sharedData.simulationHeight));
        auto yCeil  = static_cast<long>(fmod(ceil (y(i)), sharedData.simulationHeight));
        Decimal yT = fmod(y(i), 1);

        auto xFloor = static_cast<long>(fmod(floor(x(i)), sharedData.simulationWidth));
        auto xCeil  = static_cast<long>(fmod(ceil (x(i)), sharedData.simulationWidth));
        Decimal xT = fmod(x(i), 1);

        jassert(timestampsFloor < frameBuffer.size());
        jassert(timestampsCeil  < frameBuffer.size());
        jassert(yFloor >= 0 && static_cast<size_t>(yFloor) < frameBuffer.at(timestampsFloor)->cols());
        jassert(yCeil  >= 0 && static_cast<size_t>(yCeil)  < frameBuffer.at(timestampsFloor)->cols());
        jassert(xFloor >= 0 && static_cast<size_t>(xFloor) < frameBuffer.at(timestampsFloor)->rows());
        jassert(xCeil  >= 0 && static_cast<size_t>(xCeil)  < frameBuffer.at(timestampsFloor)->rows());

        outputBuffer(i) = (1-timestampsT) * ((1-yT) * ((1-xT) * frameBuffer.at(timestampsFloor)->toDecimal(yFloor, xFloor)
                                                          +     xT  * frameBuffer.at(timestampsFloor)->toDecimal(yFloor, xCeil ))
                                                +     yT  * ((1-xT) * frameBuffer.at(timestampsFloor)->toDecimal(yCeil,  xFloor)
                                                          +     xT  * frameBuffer.at(timestampsFloor)->toDecimal(yCeil,  xCeil )))
                              +    timestampsT  * ((1-yT) * ((1-xT) * frameBuffer.at(timestampsCeil )->toDecimal(yFloor, xFloor)
                                                          +     xT  * frameBuffer.at(timestampsCeil )->toDecimal(yFloor, xCeil ))
                                                +     yT  * ((1-xT) * frameBuffer.at(timestampsCeil )->toDecimal(yCeil,  xFloor)
                                                          +     xT  * frameBuffer.at(timestampsCeil )->toDecimal(yCeil,  xCeil )));
    }
}



void
Scanner::bicubicInterpolation(const FrameList &frameBuffer, const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                              const Eigen::ArrayXX<Decimal> &y, const Eigen::ArrayXX<Decimal> &x, Eigen::ArrayXX<Decimal>& outputBuffer) {

    jassert(frameBufferTimestamps.size() == y.size() && y.size() == x.size()); // All Array sizes must match

    // interpolatedValues = RealMatrix(x.rows(), x.cols());
    if (outputBuffer.rows() < x.rows() || outputBuffer.cols() < x.cols()) {
        outputBuffer.resize(x.rows(), x.cols());
        juce::Logger::writeToLog("bicubic interpolation: buffer resized to " + juce::String(outputBuffer.size()));
    }

    // Precompute constants for better performance
    const size_t height = sharedData.simulationHeight;
    const size_t width = sharedData.simulationWidth;
    
    for (Eigen::Index i = 0; i < x.size(); i++) {
        const Decimal timestamp = frameBufferTimestamps(i);
        const Decimal yPos = y(i);
        const Decimal xPos = x(i);
        
        // Time interpolation
        const size_t tFloor = static_cast<size_t>(floor(timestamp));
        const size_t tCeil = static_cast<size_t>(ceil(timestamp));
        const Decimal t = fmod(timestamp, 1.0);
        
        // Ensure bounds
        const size_t tFloorSafe = (tFloor < frameBuffer.size()) ? tFloor : frameBuffer.size() - 1;
        const size_t tCeilSafe = (tCeil < frameBuffer.size()) ? tCeil : frameBuffer.size() - 1;
        
        // Get interpolated values for both timestamps
        const Decimal valFloor = getBicubicInterpolatedOptimized(frameBuffer, tFloorSafe, yPos, xPos, height, width);
        const Decimal valCeil = getBicubicInterpolatedOptimized(frameBuffer, tCeilSafe, yPos, xPos, height, width);
        
        // Final time interpolation
        outputBuffer(i) = (1.0 - t) * valFloor + t * valCeil;
    }
}

// Optimized bicubic interpolation for a single timestamp
Decimal Scanner::getBicubicInterpolatedOptimized(const FrameList &frameBuffer, size_t frameBufferTimestamp, 
                                                 Decimal y, Decimal x, size_t height, size_t width) {
    // This implements Catmull-Rom spline interpolation, not standard bicubic
    // First interpolate in Y direction, then in X direction
    
    // Get the 4x4 grid of surrounding points with bounds checking
    Decimal yValues[4];
    Decimal xValues[4];
    
    // Calculate integer and fractional parts
    const Decimal yFloor = floor(y);
    const Decimal xFloor = floor(x);
    const Decimal yFrac = y - yFloor;
    const Decimal xFrac = x - xFloor;
    
    // Get 4 points in Y direction for each X position
    for (int dx = -1; dx <= 2; dx++) {
        const long xIdx = static_cast<long>(xFloor) + dx;
        const long xSafe = (xIdx < 0) ? (xIdx + width) : (xIdx >= static_cast<long>(width)) ? (xIdx - width) : xIdx;
        
        // Get 4 points in Y direction for this X position
        for (int dy = -1; dy <= 2; dy++) {
            const long yIdx = static_cast<long>(yFloor) + dy;
            const long ySafe = (yIdx < 0) ? (yIdx + height) : (yIdx >= static_cast<long>(height)) ? (yIdx - height) : yIdx;
            yValues[dy + 1] = frameBuffer.at(frameBufferTimestamp)->toDecimal(ySafe, xSafe);
        }
        
        // Interpolate in Y direction using Catmull-Rom
        xValues[dx + 1] = catmullRomInterpolate(yValues[0], yValues[1], yValues[2], yValues[3], yFrac);
    }
    
    // Interpolate in X direction using Catmull-Rom
    return catmullRomInterpolate(xValues[0], xValues[1], xValues[2], xValues[3], xFrac);
}


// Legacy functions for backward compatibility - now use optimized versions
Decimal Scanner::getBicubicInterpolated(const FrameList &frameBuffer, Decimal frameBufferTimestamp, Decimal y, Decimal x) {
    const size_t timestampFloor = static_cast<size_t>(floor(frameBufferTimestamp));
    const size_t timestampCeil = static_cast<size_t>(ceil(frameBufferTimestamp));
    const Decimal t = fmod(frameBufferTimestamp, 1.0);

    const Decimal valFloor = getBicubicInterpolatedOptimized(frameBuffer, timestampFloor, y, x, 
                                                            sharedData.simulationHeight, sharedData.simulationWidth);
    const Decimal valCeil = getBicubicInterpolatedOptimized(frameBuffer, timestampCeil, y, x, 
                                                           sharedData.simulationHeight, sharedData.simulationWidth);

    return (1.0 - t) * valFloor + t * valCeil;
}

Decimal Scanner::getBicubicInterpolated(const FrameList &frameBuffer, size_t frameBufferTimestamp, Decimal y, Decimal x) {
    return getBicubicInterpolatedOptimized(frameBuffer, frameBufferTimestamp, y, x, 
                                          sharedData.simulationHeight, sharedData.simulationWidth);
}

Decimal Scanner::getBicubicInterpolated(const FrameList &frameBuffer, size_t frameBufferTimestamp, Eigen::Index y, Decimal x) {
    return getBicubicInterpolatedOptimized(frameBuffer, frameBufferTimestamp, static_cast<Decimal>(y), x, 
                                          sharedData.simulationHeight, sharedData.simulationWidth);
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


void
Scanner::getValuesLine(const Eigen::ArrayXX<Decimal> &position0to1, InterpolationFunc interpolation,
                       const ModulationData &modulationData, Eigen::ArrayXX<Decimal>& outputBuffer) {
    lineOfInterestX = sharedData.parameters->lineOfInterestX->getModulated(modulationData).cwiseMin(1 - 1e-9).cwiseMax(-1 + 1e-9);
    lineOfInterestY = -sharedData.parameters->lineOfInterestY->getModulated(modulationData).cwiseMin(1 - 1e-9).cwiseMax(-1 + 1e-9);
    lineOfInterestLength = sharedData.parameters->lineOfInterestLength->getModulated(modulationData);
    lineOfInterestRotation = sharedData.parameters->lineOfInterestRotation->getModulated(modulationData) / 360 * juce::MathConstants<Decimal>::twoPi;

    // Shared Data
    voiceData->lineOfInterestX = lineOfInterestX(Eigen::last);
    voiceData->lineOfInterestY = lineOfInterestY(Eigen::last);
    voiceData->lineOfInterestLength = lineOfInterestLength(Eigen::last);
    voiceData->lineOfInterestRotation = lineOfInterestRotation(Eigen::last);

    auto timestamps = sharedData.frameBufferTimestamps.replicate(1, position0to1.cols());

    xEnd = lineOfInterestLength * lineOfInterestRotation.cos();
    yEnd = lineOfInterestLength * lineOfInterestRotation.sin();
    xStart = -xEnd;
    yStart = -yEnd;

    endClipDivisor = clipDivisor(xEnd, yEnd, lineOfInterestX, lineOfInterestY);
    startClipDivisor = clipDivisor(xStart, yStart, lineOfInterestX, lineOfInterestY);

    xEnd = xEnd / endClipDivisor + lineOfInterestX;
    yEnd = yEnd / endClipDivisor + lineOfInterestY;
    xStart = xStart / startClipDivisor + lineOfInterestX;
    yStart = yStart / startClipDivisor + lineOfInterestY;

    x = (1-position0to1).colwise() * xStart + position0to1.colwise() * xEnd;
    y = (1-position0to1).colwise() * yStart + position0to1.colwise() * yEnd;

    xScaled = (x + 1) / 2 * sharedData.simulationWidth;
    yScaled = (y + 1) / 2 * sharedData.simulationHeight;

    interpolation(sharedData.frameBuffer, timestamps, yScaled, xScaled, outputBuffer);
}


void
Scanner::getValuesCircle(const Eigen::ArrayXX<Decimal> &position0to1, InterpolationFunc interpolation,
                         const ModulationData &modulationData, Eigen::ArrayXX<Decimal>& outputBuffer) {

    // TODO extract locals
    Eigen::ArrayX<Decimal> circleX = sharedData.parameters->lineOfInterestX->getModulated(modulationData).cwiseMin(1 - 1e-9).cwiseMax(-1 + 1e-9);
    Eigen::ArrayX<Decimal> circleY = -sharedData.parameters->lineOfInterestY->getModulated(modulationData).cwiseMin(1 - 1e-9).cwiseMax(-1 + 1e-9);
    Eigen::ArrayX<Decimal> circleWidth = sharedData.parameters->circleOfInterestWidth->getModulated(modulationData);
    Eigen::ArrayX<Decimal> circleHeight = sharedData.parameters->circleOfInterestHeight->getModulated(modulationData);
    Eigen::ArrayX<Decimal> circleRotation = sharedData.parameters->lineOfInterestRotation->getModulated(modulationData) / 360 * juce::MathConstants<Decimal>::twoPi;
    Eigen::ArrayX<Decimal> circleFraction = sharedData.parameters->circleOfInterestFraction->getModulated(modulationData);

    // Shared Data
    voiceData->lineOfInterestX = circleX(Eigen::last);
    voiceData->lineOfInterestY = circleY(Eigen::last);
    voiceData->circleOfInterestWidth = circleWidth(Eigen::last);
    voiceData->circleOfInterestHeight = circleHeight(Eigen::last);
    voiceData->lineOfInterestRotation = circleRotation(Eigen::last);
    voiceData->circleOfInterestFraction = circleFraction(Eigen::last);

    auto timestamps = sharedData.frameBufferTimestamps.replicate(1, position0to1.cols());

    // Circle setup
    Eigen::ArrayX<Decimal> alpha = juce::MathConstants<Decimal>::twoPi * circleFraction * position0to1 + juce::MathConstants<Decimal>::pi * (1 - circleFraction);
    Eigen::ArrayX<Decimal> x = circleWidth  * alpha.cos();
    Eigen::ArrayX<Decimal> y = circleHeight * alpha.sin();

    // Rotation
    Eigen::ArrayX<Decimal> sinRotation = circleRotation.sin();
    Eigen::ArrayX<Decimal> cosRotation = circleRotation.cos();
    Eigen::ArrayX<Decimal> xRotated = x * cosRotation - y * sinRotation;
    Eigen::ArrayX<Decimal> yRotated = x * sinRotation + y * cosRotation;

    // Clipping
    auto divisor = clipDivisor(xRotated, yRotated, circleX, circleY);
    xRotated /= divisor;
    yRotated /= divisor;

    // Moving
    xRotated += circleX;
    yRotated += circleY;

    // Simulation scaling
    xRotated = (xRotated + 1) / 2 * sharedData.simulationWidth;
    yRotated = (yRotated + 1) / 2 * sharedData.simulationHeight;

    interpolation(sharedData.frameBuffer, timestamps, yRotated, xRotated, outputBuffer);
}
