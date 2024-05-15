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

SimulationDisplay::~SimulationDisplay() = default;

void SimulationDisplay::paint(juce::Graphics &g) {
    g.fillAll (juce::Colours::black);

    drawSimulation(g);

    g.setColour (juce::Colours::white);
    g.setFont (8.0f);
    g.drawText (std::to_string(std::chrono::system_clock::now().time_since_epoch().count()), getLocalBounds(), juce::Justification::topRight, true);
}

void SimulationDisplay::resized() {
}

void SimulationDisplay::timerCallback() {
    repaint();
}

void SimulationDisplay::drawSimulation(juce::Graphics &g) {
    const auto frame = sharedData.getSimulationDisplayFrame();
    if (frame.empty())
        return;

    const size_t w = sharedData.simWidth;
    const size_t h = sharedData.simHeight;

    const num vx = static_cast<num>(getWidth()) / static_cast<num>(w);
    const num vy = static_cast<num>(getHeight()) / static_cast<num>(h);

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            num v = frame.at(x * w + y);
            int rgb = std::min(255, static_cast<int>(std::round(std::pow(std::abs(v), 0.66) * 255)));
            // if (rgb < 2)
                // continue;
            // int cR = rgb * ((rgbMask & 0xff0000) >> 16) / 255;
            // int cG = rgb * ((rgbMask & 0x00ff00) >> 8) / 255;
            // int cB = rgb * (rgbMask & 0x0000ff) / 255;
            g.setColour(juce::Colour(rgb, rgb, rgb));
            // fill rectangle
            g.fillRect(juce::Rectangle(static_cast<num>(x) * vx, static_cast<num>(y) * vy, vx, vy));
        }
    }

}
