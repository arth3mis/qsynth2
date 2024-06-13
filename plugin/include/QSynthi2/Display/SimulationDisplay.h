#ifndef SIMULATIONDISPLAY_H
#define SIMULATIONDISPLAY_H

#include <QSynthi2/Juce.h>
#include <QSynthi2/types.h>

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

    float simulationXToScreenX(Decimal simulationX) const;
    float simulationYToScreenY(Decimal simulationY) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimulationDisplay)
};

#endif //SIMULATIONDISPLAY_H
