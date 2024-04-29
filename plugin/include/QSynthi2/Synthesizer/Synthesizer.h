#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "Voice.h"
#include "Oscillator.h"


class Synthesizer {
public:

    Synthesizer(size_t numVoices = 12) {
        setNumVoices(numVoices);
    }

    void setNumVoices(size_t numVoices) {
        while (voices.size() < numVoices) {
            voices.push_back(std::make_shared<Voice>(createOscillator()));
        }

        while (voices.size() > numVoices) {
            voices.erase(voices.begin());
        }
    }

protected:

    virtual std::shared_ptr<Oscillator> createOscillator() = 0;

    // All voices
    std::vector<std::shared_ptr<Voice>> voices;

};