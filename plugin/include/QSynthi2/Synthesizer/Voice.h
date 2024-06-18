#pragma once

#include "QSynthi2/types.h"
#include "QSynthi2/Juce.h"
#include "QSynthi2/Parameter/Modulation.h"
#include "Sonifier.h"
#include "QSynthi2/Synthesizer/VoiceData.h"
#include "QSynthi2/Synthesizer/ADSR.h"

class Voice : public juce::MPESynthesiserVoice {
public:

    explicit Voice(const std::shared_ptr<VoiceData>& voiceData);

    static Decimal frequencyToModulationValue(double frequency);

    void noteStarted() override;

    void noteStopped (bool allowTailOff) override;

    void notePitchbendChanged() override;

    void noteTimbreChanged() override;

    void notePressureChanged() override;

    void noteKeyStateChanged() override {}

    void prepareToPlay(Decimal sampleRate, int samplesPerBlock);

    // Does the preprocessing of midi and modulation sources
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    using MPESynthesiserVoice::renderNextBlock; // TODO: Why? Necessary?

    // Generates the audio
    Eigen::ArrayX<Decimal> generateNextBlock();

    ModulationData* getModulationData() {
        return &modulationData;
    }

    [[nodiscard]] bool isActiveThisBlock() const;


protected:

    bool activeThisBlock = false;

    ModulationData modulationData;
    std::shared_ptr<VoiceData> voiceData;

    juce::SmoothedValue<Decimal> velocity;
    juce::SmoothedValue<Decimal> x;
    juce::SmoothedValue<Decimal> y;
    juce::SmoothedValue<Decimal> z;

    ADSR envelope1;

    Sonifier sonifier;

    juce::dsp::IIR::Filter<Decimal> dcOffsetFilter;

};