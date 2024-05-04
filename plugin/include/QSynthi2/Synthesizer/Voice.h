#pragma once

#include <utility>

#include "juce_audio_processors/juce_audio_processors.h"
#include "Oscillator.h"

class Voice : public juce::MPESynthesiserVoice {
public:

    Voice() { }


    void noteStarted() override {
        jassert (currentlyPlayingNote.isValid());
        jassert (currentlyPlayingNote.keyState == juce::MPENote::keyDown
                 || currentlyPlayingNote.keyState == juce::MPENote::keyDownAndSustained);

        gain = 1.0;
        // TODO: Start playing

    }


    void noteStopped (bool allowTailOff) override {
        jassert (currentlyPlayingNote.keyState == juce::MPENote::off);

        gain = 0.0;

        if (!allowTailOff) {
            // TODO: Instant note Off
            return;
        }


    }


    void notePressureChanged() override {
        // TODO: react
    }



    void notePitchbendChanged() override {
        // TODO: react
    }



    void noteTimbreChanged() override {
        // TODO: react
    }



    void noteKeyStateChanged() override {}



    void setCurrentSampleRate (double newRate) override {
        if (juce::approximatelyEqual(currentSampleRate, newRate)) return;

        noteStopped(false);
        currentSampleRate = newRate;
        // TODO: react
    }



    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override {
        // TODO: fill outputBuffer with samples from getNextSample()

        for (int sampleIndex = startSample; sampleIndex < startSample + numSamples; sampleIndex++) {

            double sample = getNextSample();

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

    double phase = 0.0;
    double gain = 0.0;

    double getNextSample() noexcept {

        phase += currentlyPlayingNote.getFrequencyInHertz() / currentSampleRate * juce::MathConstants<double>::twoPi;

        return gain * tanh(sin(phase));
        // TODO: Implement
    }

};