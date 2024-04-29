#pragma once

#include <utility>

#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_dsp/juce_dsp.h"
#include "QSynthi2/List/list.h"
#include <functional>

class ProcessingBlock : public juce::dsp::AudioBlock<double>, public juce::MidiBuffer {

public:

    ProcessingBlock(int _oversampling = 1) {
        setOversampling(_oversampling);


    }

    ProcessingBlock startProcessing(juce::dsp::AudioBlock<float> input, juce::MidiBuffer& midiBuffer) {
        jassert(!startedProcessing); // Processing has already started!



        return *this;
    }

    void prepareToPlay(double _sampleRate, size_t numSamples, size_t numChannels) {
        sampleRate = _sampleRate;



    }

    void finishProcessing(juce::dsp::AudioBlock<float> output) {
        jassert(!startProcessing());
        jassert(getNumChannels() > 1 && getNumSamples() > 1);
        jassert(output.getNumChannels() == samples.size());
        jassert(output.getNumSamples() == samples[0].size());


    }

    void setOversampling(int _oversampling, bool linearPhase) {
        jassert(oversampling == 1 || oversampling == 2 || oversampling == 4 || oversampling == 8 || oversampling == 16); // These are the allowed amounts
        oversampling = _oversampling;
    }

    double getSampleRate() const {
        return sampleRate * oversampling;
    }


    /**
     * @return number of channels
     */
    size_t getNumChannels() {
        return samples.size();
    }


    /**
     * @return number of Samples per Channel.
     */
    size_t getNumSamples() {
        jassert(getNumChannels() > 0);
        return samples[0].size();
    }

protected:

    bool startedProcessing = false; // For assertions

    double sampleRate;
    int oversampling;
    List<List<double>> samples;




};