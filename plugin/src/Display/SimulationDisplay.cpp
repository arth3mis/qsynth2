//
// Created by art on 15.05.24.
//
#include "QSynthi2/Display/SimulationDisplay.h"
#include <chrono>
#include <QSynthi2/Data.h>

extern Data sharedData;


SimulationDisplay::SimulationDisplay() {
    startTimerHz(30);
}

SimulationDisplay::~SimulationDisplay() {
}

void SimulationDisplay::paint(juce::Graphics &g) {
    g.fillAll (juce::Colours::black);

    drawSimulation(g);

    g.setColour (juce::Colours::white);
    g.setFont (12.0f);
    g.drawText (std::to_string(std::chrono::system_clock::now().time_since_epoch().count()), getLocalBounds(), juce::Justification::centredBottom, true);
}

void SimulationDisplay::resized() {
}

void SimulationDisplay::timerCallback() {
    repaint();
}

void SimulationDisplay::drawSimulation(juce::Graphics &g) {
    const auto frame = sharedData.getSimulationDisplayFrame();
    if (frame.size() == 0) {
        return;
    }

    const size_t w = SIM_W;
    const size_t h = SIM_H;

    const float vx = static_cast<float>(getWidth()) / static_cast<float>(w);
    const float vy = static_cast<float>(getHeight()) / static_cast<float>(h);

    const float xOverlap = vx/10;
    const float yOverlap = vy/10;

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            num v = std::abs(frame(y, x));
            int rgb = std::min(255, static_cast<int>(std::round(std::pow(std::abs(v), 0.66) * 255)));
            g.setColour(juce::Colour(rgb, rgb, rgb));
            // fill rectangle
            g.fillRect(juce::Rectangle(
                static_cast<float>(x) * vx - xOverlap,
                static_cast<float>(y) * vy - yOverlap,
                vx + xOverlap * 2,
                vy + yOverlap * 2));
        }
    }

}
