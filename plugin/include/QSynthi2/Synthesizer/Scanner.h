#pragma once

#include "QSynthi2/types.h"
#include "QSynthi2/Parameter/ParameterCollection.h"
#include "QSynthi2/Synthesizer/VoiceData.h"

class Scanner {
public:

    explicit Scanner(std::shared_ptr<VoiceData> voiceData);

    Eigen::ArrayXX<Decimal> getValuesAt(const Eigen::ArrayXX<Decimal> &position0to1, std::function<Eigen::ArrayXX<Decimal>(const FrameList &frameBuffer,
                                                                                                                           const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                                                                                           const Eigen::ArrayXX<Decimal> &y,
                                                                                                                           const Eigen::ArrayXX<Decimal> &x)>, const ModulationData &modulationData);

    static Eigen::ArrayXX<Decimal> noInterpolation(const FrameList &frameBuffer,
                                                   const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                   const Eigen::ArrayXX<Decimal> &y,
                                                   const Eigen::ArrayXX<Decimal> &x);

    static Eigen::ArrayXX<Decimal> linearInterpolation(const FrameList &frameBuffer,
                                                   const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                   const Eigen::ArrayXX<Decimal> &y,
                                                   const Eigen::ArrayXX<Decimal> &x);

    static Eigen::ArrayXX<Decimal> bicubicInterpolation(const FrameList &frameBuffer,
                                                        const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                        const Eigen::ArrayXX<Decimal> &y,
                                                        const Eigen::ArrayXX<Decimal> &x);

    inline static Decimal getBicubicInterpolated(const FrameList &frameBuffer, Decimal frameBufferTimestamp, Decimal      y, Decimal x);
    inline static Decimal getBicubicInterpolated(const FrameList &frameBuffer, size_t  frameBufferTimestamp, Decimal      y, Decimal x);
    inline static Decimal getBicubicInterpolated(const FrameList &frameBuffer, size_t  frameBufferTimestamp, Eigen::Index y, Decimal x);

    template <int pointNumber>
    inline static Eigen::Index bicubicIndex(Decimal interpolationIndex, size_t size);

    template <int pointNumber>
    inline static Decimal bicubicCoefficient(Decimal v0, Decimal v1, Decimal v2, Decimal v3);

    void prepareToPlay(Decimal newSampleRate);

    void restart();

private:

    Decimal sampleRate = 0;

    std::shared_ptr<VoiceData> voiceData;

    static Eigen::ArrayXX<Decimal> toDecimal(const Eigen::ArrayXX<Complex> &simulationValues);
};