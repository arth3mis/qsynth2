#pragma once

#include "QSynthi2/PluginProcessor.h"
#include "QSynthi2/Display/SimulationDisplay.h"

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

    juce::TextButton resetButton{"Reset Simulation"};
    juce::TextButton videoLoadButton{"Load video / image"};
    juce::TextButton camLoadButton{"Load webcam"};
    juce::FileChooser fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
