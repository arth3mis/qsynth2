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
    if (!frame || frame->rows() == 0) {
        return;
    }

    const size_t w = std::min(static_cast<size_t>(sharedData.simulationWidth), static_cast<size_t>(500));
    const size_t h = std::min(static_cast<size_t>(sharedData.simulationHeight), static_cast<size_t>(500));
    const auto fw = static_cast<float>(w);
    const auto fh = static_cast<float>(h);
    const Decimal dx = static_cast<Decimal>(sharedData.simulationWidth) / fw;
    const Decimal dy = static_cast<Decimal>(sharedData.simulationHeight) / fh;

    const float vx = static_cast<float>(getWidth()) / fw;
    const float vy = static_cast<float>(getHeight()) / fh;

    const float xOverlap = vx/10;
    const float yOverlap = vy/10;

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            Decimal v = frame->toDecimalDisplay(static_cast<long>(y * dy), static_cast<long>(x * dx));
            int rgb = std::min(255, static_cast<int>(std::round(std::pow(std::abs(v), 0.45) * 255)));
            g.setColour(juce::Colour(rgb, rgb, rgb));
            // fill rectangle
            g.fillRect(juce::Rectangle(
                static_cast<float>(x) * vx - xOverlap,
                static_cast<float>(y) * vy - yOverlap,
                vx + xOverlap * 2,
                vy + yOverlap * 2));
        }
    }

    // draw barrier TODO y barriers
    if (sharedData.barrierX >= -1) {
        const float barrierX = static_cast<float>(sharedData.barrierX) * fw/2 + fw/2;
        const List<V2>& slits = sharedData.barrierSlits;
        g.setColour(juce::Colour(150, 190, 255));
        for (int i = 0; i < h; ++i) {
            bool insideSlit = false;
            for (const auto& slit : slits) {
                if (i >= slit.x * fh/2 + fh/2 && i < slit.y * fh/2 + fh/2) {
                    insideSlit = true;
                    break;
                }
            }
            if (!insideSlit) {
                g.fillRect(juce::Rectangle(
                    (barrierX - static_cast<float>(sharedData.barrierWidth/2)) * vx - xOverlap,
                    static_cast<float>(i) * vy - yOverlap,
                    static_cast<float>(sharedData.barrierWidth) * vx + xOverlap * 2,
                    vy + yOverlap * 2));
            }
        }
    }
}

void SimulationDisplay::drawScanlines(juce::Graphics &g) const {
    for (const auto& voiceData : sharedData.voiceData) {
        if (!voiceData->isActive()) continue;

        const auto line = juce::Line(
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