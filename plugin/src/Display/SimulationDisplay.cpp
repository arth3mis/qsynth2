#include "QSynthi2/Display/SimulationDisplay.h"
#include <chrono>
#include <QSynthi2/Data.h>

extern Data sharedData;


SimulationDisplay::SimulationDisplay() {
    startTimerHz(30);
}

SimulationDisplay::~SimulationDisplay() = default;

void simulationXToScreenX(Decimal start);

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

    const int w = static_cast<int>(sharedData.simulationWidth);
    const int h = static_cast<int>(sharedData.simulationHeight);
    const auto fw = static_cast<float>(w);
    const auto fh = static_cast<float>(h);

    const float vx = static_cast<float>(getWidth()) / fw;
    const float vy = static_cast<float>(getHeight()) / fh;

    const float xOverlap = vx/10;
    const float yOverlap = vy/10;

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            Decimal v = frame->toDecimalDisplay(y, x);
            uint8_t rgb = static_cast<uint8_t>(std::min(255.0, std::round(v * 255)));
            g.setColour(juce::Colour(rgb, rgb, rgb));
            // fill rectangle
            g.fillRect(juce::Rectangle(
                static_cast<float>(x) * vx - xOverlap,
                static_cast<float>(y) * vy - yOverlap,
                vx + xOverlap * 2,
                vy + yOverlap * 2));
        }
    }

    // draw barrier
    if (sharedData.barrierType != BARRIER_NONE && sharedData.barrierWidth >= 1) {
        const List<V2>& slits = sharedData.barrierSlits;
        g.setColour(juce::Colour(150, 190, 255));
        if (sharedData.barrierType == BARRIER_VERTICAL) {
            const float barrierOffset = static_cast<float>(sharedData.barrierOffset) * fw/2 + fw/2;
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
                        (barrierOffset - static_cast<float>(sharedData.barrierWidth/2)) * vx - xOverlap,
                        static_cast<float>(i) * vy - yOverlap,
                        static_cast<float>(sharedData.barrierWidth) * vx + xOverlap * 2,
                        vy + yOverlap * 2));
                }
            }
        } else if (sharedData.barrierType == BARRIER_HORIZONTAL) {
            const float barrierOffset = static_cast<float>(sharedData.barrierOffset) * fh/2 + fh/2;
            for (int i = 0; i < w; ++i) {
                bool insideSlit = false;
                for (const auto& slit : slits) {
                    if (i >= slit.x * fw/2 + fw/2 && i < slit.y * fw/2 + fw/2) {
                        insideSlit = true;
                        break;
                    }
                }
                if (!insideSlit) {
                    g.fillRect(juce::Rectangle(
                        static_cast<float>(i) * vx - xOverlap,
                        (barrierOffset - static_cast<float>(sharedData.barrierWidth/2)) * vy - yOverlap,
                        vx + xOverlap * 2,
                        static_cast<float>(sharedData.barrierWidth) * vy + yOverlap * 2));
                }
            }
        }
    }
}

void SimulationDisplay::drawScanlines(juce::Graphics &g) const {
    for (const auto& voiceData : sharedData.voiceData) {
        if (!voiceData->isActive()) continue;


        const juce::Path path = voiceData->lineOfInterestShape == 0 ? lineOfInterest(voiceData) : circleOfInterest(voiceData);


        auto gain = static_cast<float>(voiceData->gain);
        auto hue = fmod(static_cast<float>(std::log2(voiceData->frequency / 55.0) / 3), 1.f);

        g.setColour(juce::Colour(hue, 0.6f, 1.f, 0.1f * std::pow(gain, 2.f)));
        g.strokePath(path, juce::PathStrokeType(10));

        g.setColour(juce::Colour(hue, 0.7f, 1.f, 0.1f * std::pow(gain, 1.7f)));
        g.strokePath(path, juce::PathStrokeType(8));

        g.setColour(juce::Colour(hue, 0.8f, 1.f, 0.2f * std::pow(gain, 1.5f)));
        g.strokePath(path, juce::PathStrokeType(6));

        g.setColour(juce::Colour(hue, 0.8f, 1.f, 0.2f * std::pow(gain, 1.3f)));
        g.strokePath(path, juce::PathStrokeType(4));

        g.setColour(juce::Colour(hue, 0.8f, 1.f, 0.2f * gain));
        g.strokePath(path, juce::PathStrokeType(2));

        g.setColour(juce::Colour(hue, 0.05f, 1.f, 1.f * std::pow(gain, 0.5f)));
        g.strokePath(path, juce::PathStrokeType(1));

    }
}



static Decimal clipDivisor(Decimal x, Decimal y, Decimal anchorX, Decimal anchorY) {
    return std::max((1 / ( 1 - anchorX) * x),
           std::max((1 / (-1 - anchorX) * x),
           std::max((1 / ( 1 - anchorY) * y),
           std::max((1 / (-1 - anchorY) * y), 1.0))));
}

static Eigen::ArrayX<Decimal> clipDivisor(const Eigen::ArrayX<Decimal> &x, const Eigen::ArrayX<Decimal> &y, Decimal anchorX, Decimal anchorY) {
    return  (1 / ( 1 - anchorX) * x).cwiseMax(
            (1 / (-1 - anchorX) * x)).cwiseMax(
            (1 / ( 1 - anchorY) * y)).cwiseMax(
            (1 / (-1 - anchorY) * y)).cwiseMax(
            1);
}



juce::Path SimulationDisplay::lineOfInterest(const std::shared_ptr<VoiceData> &voiceData) const {
    Decimal lineOfInterestX = voiceData->lineOfInterestX;
    Decimal lineOfInterestY = voiceData->lineOfInterestY;
    Decimal lineOfInterestLength = voiceData->lineOfInterestLength;
    Decimal lineOfInterestRotation = voiceData->lineOfInterestRotation;

    // Line positions
    Decimal xEnd = lineOfInterestLength * cos(lineOfInterestRotation);
    Decimal yEnd = lineOfInterestLength * sin(lineOfInterestRotation);

    Decimal xStart = -xEnd;
    Decimal yStart = -yEnd;

    // Clipping
    auto endClipDivisor = clipDivisor(xEnd, yEnd, lineOfInterestX, lineOfInterestY);
    auto startClipDivisor = clipDivisor(xStart, yStart, lineOfInterestX, lineOfInterestY);

    xEnd = xEnd / endClipDivisor + lineOfInterestX;
    yEnd = yEnd / endClipDivisor + lineOfInterestY;
    xStart = xStart / startClipDivisor + lineOfInterestX;
    yStart = yStart / startClipDivisor + lineOfInterestY;

    // Path creation
    auto path = juce::Path();

    path.startNewSubPath(simulationXToScreenX(xStart),
                         simulationYToScreenY(yStart));

    path.lineTo(simulationXToScreenX(xEnd),
                simulationYToScreenY(yEnd));

    path.closeSubPath();

    return path;
}


const Eigen::Index CIRCLE_RESOLUTION = 128;

juce::Path SimulationDisplay::circleOfInterest(const std::shared_ptr<VoiceData> &voiceData) const {
    Decimal circleX = voiceData->lineOfInterestX;
    Decimal circleY = voiceData->lineOfInterestY;
    Decimal circleWidth = voiceData->circleOfInterestWidth;
    Decimal circleHeight = voiceData->circleOfInterestHeight;
    Decimal circleRotation = voiceData->lineOfInterestRotation;
    Decimal circleFraction = voiceData->circleOfInterestFraction;
    Eigen::Array<Decimal, CIRCLE_RESOLUTION, 1> position0to1 = Eigen::Array<Decimal, CIRCLE_RESOLUTION, 1>::LinSpaced(CIRCLE_RESOLUTION, 0.0, 1.0);

    Eigen::Array<Decimal, CIRCLE_RESOLUTION, 1> alpha = juce::MathConstants<Decimal>::twoPi * circleFraction * position0to1 + juce::MathConstants<Decimal>::pi * (1 - circleFraction);
    Eigen::Array<Decimal, CIRCLE_RESOLUTION, 1> x = circleWidth * alpha.cos();
    Eigen::Array<Decimal, CIRCLE_RESOLUTION, 1> y = circleHeight * alpha.sin();

    // Rotation
    Eigen::Array<Decimal, CIRCLE_RESOLUTION, 1> xRotated = x * cos(circleRotation) - y * sin(circleRotation);
    Eigen::Array<Decimal, CIRCLE_RESOLUTION, 1> yRotated = x * sin(circleRotation) + y * cos(circleRotation);

    // Clipping
    auto divisor = clipDivisor(xRotated, yRotated, circleX, circleY);
    xRotated /= divisor;
    yRotated /= divisor;

    // Moving
    xRotated += circleX;
    yRotated += circleY;

    // Path creation
    auto path = juce::Path();

    path.startNewSubPath(simulationXToScreenX(xRotated(0)),
                         simulationYToScreenY(yRotated(0)));

    for (Eigen::Index i = 1; i < CIRCLE_RESOLUTION; i++) {
        path.lineTo(simulationXToScreenX(xRotated(i)),
                    simulationYToScreenY(yRotated(i)));
    }

    if (juce::approximatelyEqual(circleFraction, 1.0)) {
        path.closeSubPath();
    }

    return path.createPathWithRoundedCorners(3);
}



float SimulationDisplay::simulationXToScreenX(Decimal simulationX) const {
    return static_cast<float>((simulationX + 1) / 2 * getWidth());
}

float SimulationDisplay::simulationYToScreenY(Decimal simulationY) const {
    return static_cast<float>((simulationY + 1) / 2 * getHeight());
}