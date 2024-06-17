#pragma once

#include "Scanner.h"
#include "QSynthi2/Parameter/ParameterCollection.h"
#include "VoiceData.h"

class Sonifier {
public:

    Sonifier(std::shared_ptr<VoiceData> voiceData);

    Eigen::ArrayX<Decimal> generateNextBlock(const std::function<Eigen::ArrayX<Decimal>(const Eigen::ArrayX<Decimal>&, Scanner &scanner, const ModulationData&)>& sonificationMethod, const ModulationData& modulationData);

    static Eigen::ArrayX<Decimal> audification(const Eigen::ArrayX<Decimal> &phases0to1, Scanner &scanner, const ModulationData &modulationData);

    static Eigen::ArrayX<Decimal> timbreMapping(const Eigen::ArrayX<Decimal> &phases0to1, Scanner &scanner, const ModulationData &modulationData);

    void prepareToPlay(Decimal newSampleRate, int samplesPerBlock);

    void restart();



protected:

    Decimal phase0to1 = 0;

    Scanner scanner;

    Decimal sampleRate = 0;
    int samplesPerBlock = 0;
};