#include "QSynthi2/Synthesizer/Sonifier.h"


Sonifier::Sonifier(std::shared_ptr<VoiceData> voiceData) : scanner(voiceData) {

}



Eigen::ArrayX<Decimal> Sonifier::generateNextBlock(const ModulationData &modulationData) {
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

    auto scannerValues = scanner.getValuesAt(phases, modulationData);
    jassert(scannerValues.cols() == 1); // Scanner returned too many Columns

    return scannerValues;
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
