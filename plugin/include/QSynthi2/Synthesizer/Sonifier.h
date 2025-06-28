#pragma once

#include "Scanner.h"
#include "QSynthi2/Parameter/ParameterCollection.h"
#include "VoiceData.h"

class Sonifier {
public:

    explicit Sonifier(const std::shared_ptr<VoiceData>& _voiceData);

    typedef void (*SonificationFunc)(
        const Eigen::ArrayX<Decimal>&,
        Scanner&,
        const ModulationData&,
        Eigen::ArrayXX<Decimal>& outputBuffer
    );

    void generateNextBlock(SonificationFunc sonificationMethod, const ModulationData& modulationData, Eigen::ArrayX<Decimal>& outputBuffer);

    static void audification(const Eigen::ArrayX<Decimal> &phases0to1, Scanner &scanner, const ModulationData &modulationData, Eigen::ArrayXX<Decimal>& outputBuffer);

    static void timbreMapping(const Eigen::ArrayX<Decimal> &phases0to1, Scanner &scanner, const ModulationData &modulationData, Eigen::ArrayXX<Decimal>& outputBuffer);

    void prepareToPlay(Decimal newSampleRate, int samplesPerBlock);

    void restart();



protected:

    Decimal phase0to1 = 0;
    Eigen::ArrayX<Decimal> phases;

    std::shared_ptr<VoiceData> voiceData;
    Scanner scanner;

    Decimal sampleRate = 0;
    int samplesPerBlock = 0;

    // actually local variables - avoid reallocation expect when size increases
    Eigen::ArrayX<Decimal> oscillationsPerSample;
    Eigen::ArrayXX<Decimal> sonificationOutputBuffer;
    inline static Eigen::ArrayXX<Decimal> interpolatedValues{0, 0};
    inline static Eigen::ArrayX<Decimal> phases0to1Overlap{0};
    inline static Eigen::ArrayXX<Decimal> overlapValues{0, 0};
    inline static Eigen::ArrayX<Decimal> overlapMask{0};
    inline static Eigen::ArrayXX<Decimal> scannerPositions{0, 0};
    inline static Eigen::ArrayXX<Decimal> amplitudes{0, 0};
};