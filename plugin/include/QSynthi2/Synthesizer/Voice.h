#pragma once

#include <utility>

#include "juce_audio_processors/juce_audio_processors.h"
#include "QSynthi2/types.h"
#include "QSynthi2/Parameter/Modulation.h"
#include "Sonifier.h"

class Voice : public juce::MPESynthesiserVoice {
public:

    Voice() { }


    void noteStarted() override {
        jassert (currentlyPlayingNote.isValid());
        jassert (currentlyPlayingNote.keyState == juce::MPENote::keyDown
              || currentlyPlayingNote.keyState == juce::MPENote::sustained
              || currentlyPlayingNote.keyState == juce::MPENote::keyDownAndSustained);


        notePressureChanged();
        notePitchbendChanged();
        noteTimbreChanged();

        gain = 0.25;
        // TODO: Start playing

        sonifier.jumpToNote(currentlyPlayingNote);

    }


    void noteStopped (bool allowTailOff) override {
        jassert (currentlyPlayingNote.keyState == juce::MPENote::off);

        gain = 0.0;

        // TODO: Note off behaviour

        if (!allowTailOff) {
            return;
        }


    }


    void notePressureChanged() override {
        modulationData[Modulation::Sources::PRESSURE] = currentlyPlayingNote.pressure.asUnsignedFloat();
    }



    void notePitchbendChanged() override {
        // TODO: react
    }



    void noteTimbreChanged() override {
        modulationData[Modulation::Sources::TIMBRE] = currentlyPlayingNote.timbre.asUnsignedFloat();
    }



    void noteKeyStateChanged() override {}



    void setCurrentSampleRate (double newRate) override {
        if (juce::approximatelyEqual(currentSampleRate, newRate)) return;

        noteStopped(false);
        currentSampleRate = newRate;
        // TODO: react

        sonifier.prepareToPlay((num) newRate);
    }



    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override {
        // TODO: fill outputBuffer with samples from getNextSample()

        for (int sampleIndex = startSample; sampleIndex < startSample + numSamples; sampleIndex++) {

            num sample = getNextSample();

            for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++) {
                outputBuffer.addSample(channel, sampleIndex, (float) sample);
            }
        }

        if (gain == 0.0) {
            clearCurrentNote(); // Important
        }
    }

    using MPESynthesiserVoice::renderNextBlock; // TODO: Why? Necessary?



protected:

    ModulationData modulationData;

    Sonifier sonifier;

    // TODO: Temporary
    num gain = 0.0;

    num getNextSample() noexcept {
        // TODO Add envelopes, ...
        return gain * sonifier.getNextSample(getCurrentlyPlayingNote(), modulationData);
    }

};