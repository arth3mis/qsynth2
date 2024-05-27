#pragma once

#include "QSynthi2/types.h"
#include "Voice.h"

class Synthesiser : public juce::MPESynthesiser {
public:

    explicit Synthesiser(juce::MPEInstrument& instrument_) : juce::MPESynthesiser(instrument_) {

    }



    void prepareToPlay(Decimal newSampleRate, int newSamplesPerBlock) {
        juce::MPESynthesiser::setCurrentPlaybackSampleRate(newSampleRate);
        samplesPerBlock = newSamplesPerBlock;


        for (auto mpeVoice : voices) {
            if (auto* voice = dynamic_cast<Voice*>(mpeVoice)) {
                voice->prepareToPlay(newSampleRate, newSamplesPerBlock);
            }
        }

    }



    Eigen::ArrayX<Decimal> generateNextBlock() {
        auto samples = Eigen::ArrayX<Decimal>(samplesPerBlock).setZero();

        for (auto mpeVoice : voices) {
            if (mpeVoice->isActive()) { // TODO: Use more accurate method to find out if the voice was active in this block
                if (auto *voice = dynamic_cast<Voice *>(mpeVoice)) {
                    samples += voice->generateNextBlock();
                }
            }
        }

        return samples;
    }

    //

protected:

    int samplesPerBlock = 0;

};