#include "QSynthi2/PluginProcessor.h"
#include "QSynthi2/PluginEditor.h"
#include <QSynthi2/Data.h>

extern Data sharedData;


//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
    , gpe (new juce::GenericAudioProcessorEditor(p))
#if SHOW_BUFFER_FILL_PROGRESS==1
    , bufferProgressBar(new ProgressBarComponent(sharedData.simulationBufferProgressFraction, sharedData.simulationBufferProgressBarActive))
#endif
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizable (true, true);

    simDisplaySize = 700;  // is updated later based on actual window space
    controlsDisplayMinSize = 400;

    setSize(simDisplaySize + controlsDisplayMinSize, simDisplaySize);

    addAndMakeVisible(simulationDisplay);

    resetButton.onClick = [&]{ sharedData.resetSimulation = true; };
    // addAndMakeVisible(resetButton);

    gpe->setResizable(true, false);
    addAndMakeVisible(gpe);

#if SHOW_BUFFER_FILL_PROGRESS==1
    addAndMakeVisible(bufferProgressBarLabel);
    addAndMakeVisible(bufferProgressBar);
#endif
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    delete gpe;

    if (bufferProgressBar)
        delete bufferProgressBar;
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
    const int width = getWidth();
    const int height = getHeight();

    const int simSize = width - controlsDisplayMinSize < height ? width - controlsDisplayMinSize : height;

    int progressBarSpaceY = 0;
#if SHOW_BUFFER_FILL_PROGRESS==1
    progressBarSpaceY = 50;
#endif

    gpe->setTopLeftPosition(0, progressBarSpaceY);
    gpe->setSize(width - simSize, height - progressBarSpaceY);

    simulationDisplay.setTopLeftPosition(width - simSize, 0);
    simulationDisplay.setSize(simSize, simSize);

    // resetButton.setBounds(20, simDisplaySize+10, 130, 25);

#if SHOW_BUFFER_FILL_PROGRESS==1
    bufferProgressBarLabel.setBounds(20, 10, width - simSize - 100, 15);
    bufferProgressBar->setBounds(20, 30, width - simSize - 100, 15);
#endif
}
