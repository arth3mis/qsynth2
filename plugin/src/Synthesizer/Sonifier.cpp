#include "QSynthi2/Synthesizer/Sonifier.h"
#include "QSynthi2/Data.h"

extern Data sharedData;

Sonifier::Sonifier(std::shared_ptr<VoiceData> voiceData) : scanner(voiceData) {

}


Eigen::ArrayX<Decimal> Sonifier::generateNextBlock(const std::function<Eigen::ArrayX<Decimal>(const Eigen::ArrayX<Decimal> &,
                                                                                        Scanner &scanner,
                                                                                        const ModulationData &)>& sonificationMethod,
                                                   const ModulationData &modulationData) {

    jassert(modulationData.isSourceValid(ModulationData::Sources::PITCH)); // Pitch isn't already evaluated
    const auto& frequency = modulationData[ModulationData::Sources::PITCH.id];
    jassert(frequency.size() == samplesPerBlock); // Pitch wasn't set properly

    auto phases = Eigen::ArrayXX<Decimal>(samplesPerBlock, 1);

    Eigen::ArrayX<Decimal> oscillationsPerSample = frequency / sampleRate;

    for(int i = 0; i < phases.size(); i++) {
        phase0to1 += oscillationsPerSample(i);
        phase0to1 = fmod(phase0to1, 1);

        phases(i) = phase0to1;
    }

    auto sonifiedValues = sonificationMethod(phases, scanner, modulationData);
    jassert(sonifiedValues.cols() == 1); // Scanner returned too many Columns

    return sonifiedValues;
}



Eigen::ArrayX<Decimal>
Sonifier::audification(const Eigen::ArrayX<Decimal> &phases0to1, Scanner &scanner, const ModulationData &modulationData) {
    return scanner.getValuesAt(phases0to1, Scanner::bicubicInterpolation, modulationData);
}



Eigen::ArrayX<Decimal>
Sonifier::timbreMapping(const Eigen::ArrayX<Decimal> &phases0to1, Scanner &scanner, const ModulationData &modulationData) {

    auto numOvertones = static_cast<Eigen::Index>(sharedData.parameters->timbreNumberOvertones->getSingleModulated(modulationData));

    Eigen::ArrayXX<Decimal> scannerPositions;

    if (sharedData.parameters->timbreOvertoneLayout->getIndex() == 0) {
        scannerPositions = Eigen::ArrayXd::LinSpaced(numOvertones, 0, 1 - 1.0 / static_cast<Decimal>(numOvertones))
                .transpose()
                .replicate(phases0to1.rows(), 1);

    } else {
        scannerPositions = (Eigen::ArrayXd::LinSpaced(numOvertones, 1, static_cast<Decimal>(numOvertones))
                .log2() / log2(numOvertones))
                .transpose()
                .replicate(phases0to1.rows(), 1);
    }

    Eigen::ArrayXX<Decimal> amplitudes = scanner.getValuesAt(scannerPositions, Scanner::linearInterpolation, modulationData);

    // Amplitude scaling of overtones like sawtooth
    amplitudes.rowwise() /= Eigen::ArrayXd::LinSpaced(numOvertones, 1, static_cast<Decimal>(numOvertones)).transpose();

    Eigen::ArrayXX<Decimal> overtoneIndices = Eigen::ArrayXd::LinSpaced(numOvertones, 1, static_cast<Decimal>(numOvertones)).transpose().replicate(phases0to1.rows(), 1);
    Eigen::ArrayXX<Decimal> phases2d = overtoneIndices * (phases0to1 * juce::MathConstants<Decimal>::twoPi).replicate(1, numOvertones);

    Eigen::ArrayX<Decimal> output = (amplitudes * phases2d.sin()).rowwise().sum();
    return output;
}



void Sonifier::prepareToPlay(Decimal newSampleRate, int samplesPerBlock) {
    sampleRate = newSampleRate;
    this->samplesPerBlock = samplesPerBlock;

    scanner.prepareToPlay(newSampleRate);
}



void Sonifier::restart() {
    phase0to1 = 0;
    scanner.restart();
}
