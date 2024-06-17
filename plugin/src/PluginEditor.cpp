#include "QSynthi2/PluginProcessor.h"
#include "QSynthi2/PluginEditor.h"
#include <QSynthi2/Data.h>

extern Data sharedData;


//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
    , gpe (new juce::GenericAudioProcessorEditor(p))
    , fileChooser (juce::FileChooser("Choose video"))
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizable (true, true);

    simDisplaySize = 400;

    setSize(simDisplaySize, 800);

    simulationDisplay.setSize(simDisplaySize, simDisplaySize);
    addAndMakeVisible(simulationDisplay);

    resetButton.onClick = [&]{ sharedData.resetSimulation = true; };
    videoLoadButton.onClick = [&] {
        fileChooser.launchAsync(juce::FileBrowserComponent::openMode, [this](const juce::FileChooser& chooser) {
            sharedData.newSimulation = chooser.getResult().getFullPathName();
            if (sharedData.newSimulation.isNotEmpty())
                sharedData.resetSimulation = true;
        });
        // sharedData.newSimulation = "/data/media/Archive/Random/Sascha Grammel_ Hetz mich nicht - Mp4 - 720p.mp4";
    };
    addAndMakeVisible(resetButton);
    addAndMakeVisible(videoLoadButton);

    gpe->setResizable(true, false);
    addAndMakeVisible(gpe);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    delete gpe;
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
    // g.drawFittedText ("Wer das liest is doof!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    simulationDisplay.setTopLeftPosition(0, 0);

    resetButton.setBounds(20, simDisplaySize+10, 130, 25);
    videoLoadButton.setBounds(20+130+20, simDisplaySize+10, 130, 25);

    gpe->setTopLeftPosition(0, simDisplaySize+40);
    gpe->setSize(getWidth(), getHeight() - simDisplaySize);
}
