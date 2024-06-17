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

    drawScanlines(g);
}

void SimulationDisplay::resized() {
}

void SimulationDisplay::timerCallback() {
    repaint();
}

void SimulationDisplay::drawSimulation(juce::Graphics &g) const {
    const auto frame = sharedData.getSimulationDisplayFrame();
    if (frame->rows() == 0) {
        return;
    }

    const size_t w = frame->cols();
    const size_t h = frame->rows();

    const float vx = static_cast<float>(getWidth()) / static_cast<float>(w);
    const float vy = static_cast<float>(getHeight()) / static_cast<float>(h);

    const float xOverlap = vx/10;
    const float yOverlap = vy/10;

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            Decimal v = frame->toDecimal(y, x);
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

void SimulationDisplay::drawScanlines(juce::Graphics &g) const {
    for (const auto& voiceData : sharedData.voiceData) {
        if (!voiceData->isActive()) continue;

        const auto line = juce::Line<float>(
                simulationXToScreenX(voiceData->scanlineStartX),
                simulationYToScreenY(voiceData->scanlineStartY),
                simulationXToScreenX(voiceData->scanlineEndX),
                simulationYToScreenY(voiceData->scanlineEndY));


        auto gain = static_cast<float>(voiceData->gain);
        auto hue = fmod(static_cast<float>(std::log2(voiceData->frequency / 55.0) / 3), 1.f);

        g.setColour(juce::Colour(hue, 0.6f, 1.f, 0.1f * gain));
        g.drawLine(line, 10);

        g.setColour(juce::Colour(hue, 0.7f, 1.f, 0.1f * gain));
        g.drawLine(line, 8);

        g.setColour(juce::Colour(hue, 0.8f, 1.f, 0.2f * gain));
        g.drawLine(line, 6);

        g.setColour(juce::Colour(hue, 0.8f, 1.f, 0.3f * gain));
        g.drawLine(line, 4);

        g.setColour(juce::Colour(hue, 0.9f, 1.f, 0.3f * gain));
        g.drawLine(line, 2);

        g.setColour(juce::Colour(hue, 0.05f, 1.f, 1.f * std::pow(gain, 0.5f)));
        g.drawLine(line, 1);

    }
}

float SimulationDisplay::simulationXToScreenX(const Decimal simulationX) const {
    return static_cast<float>((simulationX + 1) / 2 * getWidth());
}

float SimulationDisplay::simulationYToScreenY(const Decimal simulationY) const {
    return static_cast<float>((simulationY + 1) / 2 * getHeight());
}