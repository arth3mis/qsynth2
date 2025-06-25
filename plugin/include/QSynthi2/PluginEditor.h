#pragma once

#include "QSynthi2/PluginProcessor.h"
#include "QSynthi2/Display/SimulationDisplay.h"

// TODO toggle progess bar here
#define SHOW_BUFFER_FILL_PROGRESS 1

class ProgressBarComponent : public juce::Component, private juce::Timer
{
public:
    ProgressBarComponent(std::atomic<double>& fractionRef)
        : progress(progressInternal), progressRef(fractionRef)
    {
        addAndMakeVisible(progress);
        startTimerHz(30);
    }

    void resized() override
    {
        progress.setBounds(getLocalBounds());
    }

private:
    void timerCallback() override
    {
        progressInternal = juce::jlimit(0.0, 1.0, progressRef.load());
        progress.repaint();
    }

    std::atomic<double>& progressRef;
    double progressInternal = 0.0;
    juce::ProgressBar progress;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgressBarComponent)
};


//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    juce::GenericAudioProcessorEditor* gpe;
    SimulationDisplay simulationDisplay;
    int simDisplaySize;
    int controlsDisplayMinSize;

    juce::TextButton resetButton{"Reset Simulation"};

    juce::Label bufferProgressBarLabel{{}, "Simulation buffer filled (try to keep at 100%):"};
    ProgressBarComponent* bufferProgressBar{nullptr};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
