#pragma once

#include "QSynthi2/types.h"
#include "QSynthi2/Parameter/ParameterCollection.h"
#include "QSynthi2/Synthesizer/VoiceData.h"

class Scanner {
public:

    explicit Scanner(std::shared_ptr<VoiceData> voiceData);

    Eigen::ArrayXX<Decimal> getValuesAt(const Eigen::ArrayXX<Decimal> &position0to1, const ModulationData &modulationData);

    static Eigen::ArrayXX<Decimal> getInterpolated(const FrameList &frameBuffer,
                                                   const Eigen::ArrayXX<Decimal> &frameBufferTimestamps,
                                                   const Eigen::ArrayXX<Decimal> &y,
                                                   const Eigen::ArrayXX<Decimal> &x);

    void prepareToPlay(Decimal newSampleRate);

    void restart();

private:

    Decimal sampleRate = 0;

    std::shared_ptr<VoiceData> voiceData;

    static Eigen::ArrayXX<Decimal> toDecimal(const Eigen::ArrayXX<Complex> &simulationValues);
};