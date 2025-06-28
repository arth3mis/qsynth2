#include "QSynthi2/Synthesizer/Sonifier.h"
#include "QSynthi2/Data.h"

extern Data sharedData;

Sonifier::Sonifier(const std::shared_ptr<VoiceData>& _voiceData) : voiceData(_voiceData), scanner(_voiceData) {

}


void Sonifier::generateNextBlock(SonificationFunc sonificationMethod,
                                                   const ModulationData &modulationData,
                                                   Eigen::ArrayX<Decimal>& outputBuffer) {

    const auto& frequency = sharedData.parameters->baseFrequency->getModulated(modulationData);
    jassert(frequency.unaryExpr([](Decimal f){ return !isnan(f); }).all());
    voiceData->frequency = frequency(Eigen::last);

    oscillationsPerSample = frequency / sampleRate;

    for(int i = 0; i < phases.size(); i++) {
        phase0to1 += oscillationsPerSample(i);
        phase0to1 = fmod(phase0to1, 1);

        phases(i) = phase0to1;
    }

    sonificationMethod(phases, scanner, modulationData, sonificationOutputBuffer);
    jassert(sonificationOutputBuffer.cols() == 1); // Scanner returned too many Columns

    // change ArrayXX to ArrayX
    outputBuffer = sonificationOutputBuffer;
}



void Sonifier::audification(const Eigen::ArrayX<Decimal> &phases0to1, Scanner &scanner, const ModulationData &modulationData, Eigen::ArrayXX<Decimal>& outputBuffer) {

    // https://www.desmos.com/calculator/2e1qa2mn8l
    const auto overlapAmount = sharedData.parameters->audificationSmoothing->getModulated(modulationData);

    if (overlapAmount.unaryExpr([](Decimal d){ return juce::approximatelyEqual(d, 0.0); }).any()) {
        scanner.getValuesAt(phases0to1, Scanner::bicubicInterpolation, modulationData, outputBuffer);
        return;
    }

    Eigen::ArrayX<Decimal> phases0to1Shifted = phases0to1 / (1 + overlapAmount) + (1 - 1 / (1 + overlapAmount));
    scanner.getValuesAt(phases0to1Shifted, Scanner::bicubicInterpolation, modulationData, interpolatedValues);


    phases0to1Overlap = phases0to1 / (1 + overlapAmount) + 2 * (1 - 1 / (1 + overlapAmount));
    phases0to1Overlap -= phases0to1Overlap.floor();
    scanner.getValuesAt(phases0to1Overlap, Scanner::bicubicInterpolation, modulationData, overlapValues);

    overlapMask = 0.5 - 0.5 * (juce::MathConstants<Decimal>::pi * (1 + overlapAmount) / overlapAmount * (phases0to1 / (1 + overlapAmount) - (1 - overlapAmount) / (1 + overlapAmount)).cwiseMax(0)).cos();

    outputBuffer = (1-overlapMask) * interpolatedValues + overlapMask * overlapValues;
}



void Sonifier::timbreMapping(const Eigen::ArrayX<Decimal> &phases0to1, Scanner &scanner, const ModulationData &modulationData, Eigen::ArrayXX<Decimal>& outputBuffer) {
    // TODO not working!
    auto numOvertones = static_cast<Eigen::Index>(sharedData.parameters->timbreNumberOvertones->getSingleModulated(modulationData));

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

    scanner.getValuesAt(scannerPositions, Scanner::linearInterpolation, modulationData, amplitudes);

    // Amplitude scaling of overtones like sawtooth
    amplitudes.rowwise() /= /*TODO extract (and reuse same below) LinSpaced!?*/ Eigen::ArrayXd::LinSpaced(numOvertones, 1, static_cast<Decimal>(numOvertones)).transpose();

    Eigen::ArrayXX<Decimal> overtoneIndices = Eigen::ArrayXd::LinSpaced(numOvertones, 1, static_cast<Decimal>(numOvertones)).transpose().replicate(phases0to1.rows(), 1);
    Eigen::ArrayXX<Decimal> phases2d = overtoneIndices * (phases0to1 * juce::MathConstants<Decimal>::twoPi).replicate(1, numOvertones);

    outputBuffer = (amplitudes * phases2d.sin()).rowwise().sum();
}



void Sonifier::prepareToPlay(Decimal newSampleRate, int samplesPerBlock) {
    sampleRate = newSampleRate;
    this->samplesPerBlock = samplesPerBlock;

    if (phases.size() != samplesPerBlock) {
        phases.resize(samplesPerBlock);
    }

    scanner.prepareToPlay(newSampleRate);
}



void Sonifier::restart() {
    phase0to1 = 0;
    scanner.restart();
}
