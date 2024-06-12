#pragma once

#include "QSynthi2/types.h"
#include "QSynthi2/Juce.h"
#include "QSynthi2/Parameter/Modulation.h"
#include "Sonifier.h"
#include "QSynthi2/Synthesizer/VoiceData.h"

class Voice : public juce::MPESynthesiserVoice {
public:

    Voice(const std::shared_ptr<VoiceData>& voiceData);

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

    bool isActiveThisBlock() const;


protected:

    bool activeThisBlock = false;

    ModulationData modulationData;
    std::shared_ptr<VoiceData> voiceData;

    juce::SmoothedValue<Decimal> velocity;
    juce::SmoothedValue<Decimal> frequency;
    juce::SmoothedValue<Decimal> y;
    juce::SmoothedValue<Decimal> z;

    Sonifier sonifier;

    // TODO: Temporary
    Decimal gain = 0.0;

};