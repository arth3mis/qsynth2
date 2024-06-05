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



    List<Voice*> getActiveVoices() {
        List<Voice*> activeVoices;

        for (auto mpeVoice : voices) {
            if (auto *voice = dynamic_cast<Voice *>(mpeVoice)) {
                if (voice->isActiveThisBlock()) {
                    activeVoices.append(voice);
                }
            }
        }

        return activeVoices;
    }



    Eigen::ArrayX<Decimal> generateNextBlock() {
        auto samples = Eigen::ArrayX<Decimal>(samplesPerBlock).setZero();

        getActiveVoices().forEach([&](Voice* v){ samples += v->generateNextBlock(); });

        return samples;
    }

    //

protected:

    int samplesPerBlock = 0;

};