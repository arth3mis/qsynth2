#ifndef SIMULATIONDISPLAY_H
#define SIMULATIONDISPLAY_H

#include <QSynthi2/Juce.h>
#include <QSynthi2/types.h>
#include <QSynthi2/Synthesizer/VoiceData.h>

class SimulationDisplay final : public juce::Component, juce::Timer {
public:

    SimulationDisplay();
    ~SimulationDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:

    void drawSimulation(juce::Graphics& g) const;
    void drawScanlines(juce::Graphics& g) const;

    juce::Path lineOfInterest(const std::shared_ptr<VoiceData> &voiceData) const;
    juce::Path circleOfInterest(const std::shared_ptr<VoiceData> &voiceData) const;

    [[nodiscard]] float simulationXToScreenX(Decimal simulationX) const;
    [[nodiscard]] float simulationYToScreenY(Decimal simulationY) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimulationDisplay)
};

#endif //SIMULATIONDISPLAY_H
