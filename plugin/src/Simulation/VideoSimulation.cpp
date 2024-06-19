#include "QSynthi2/Simulation/VideoSimulation.h"

#include <QSynthi2/Data.h>

extern Data sharedData;

Decimal VideoSimulationFrame::toDecimal(const long row, const long col) const {
    const Decimal R = frame(row, col)[0] / 255.0;
    const Decimal G = frame(row, col)[1] / 255.0;
    const Decimal B = frame(row, col)[2] / 255.0;
    switch (sharedData.parameters->videoToDecimalMethod->getIndex()) {
        case 1: return R;
        case 2: return G;
        case 3: return B;
        // luminance https://stackoverflow.com/a/596243
        case 0:
        default: return std::sqrt(0.299*R*R + 0.587*G*G + 0.114*B*B);
    }
}


VideoSimulation::VideoSimulation(const int targetWidth, const int targetHeight, const juce::String& filename) {
    file = filename;
    simulationWidth = targetWidth;
    simulationHeight = targetHeight;
    currentFrameIndex = 0;

    // init video capture
    if (file == "0") {
        capture.open(0);
        isCam = true;
    } else {
        capture.open(file.toStdString());
        isCam = false;
    }
    if (!capture.isOpened()) {
        juce::Logger::writeToLog("Video file could not be opened: " + file);
    }

    sharedData.videoFps = capture.get(cv::CAP_PROP_FPS);
}

VideoSimulation::~VideoSimulation() {
    capture.release();
}

void VideoSimulation::reset() {
    currentFrameIndex = 0;
}

void VideoSimulation::updateParameters(const ParameterCollection *p, const List<ModulationData *> &m) {
}

SimulationFramePointer VideoSimulation::getStartFrame() {
    if (frames.empty())
        return nullptr;
    return std::make_shared<VideoSimulationFrame>(frames[0]);
}

SimulationFramePointer VideoSimulation::getNextFrame(const Decimal timestep, const ModulationData &modulationData) {
    if (!capture.isOpened())
        return nullptr;

    sharedData.barrierX = -1.01;

    currentFrameIndex += timestep;
    auto index = static_cast<size_t>(currentFrameIndex);

    if (isCam) {
        if (currentFrameIndex >= 1) {
            if (!convertNextVideoFrame())
                return nullptr;
            if (frames.size() > 1) {
                frames[0] = frames.back();
                frames.erase(frames.begin()+1, frames.end());
            }
            currentFrameIndex = std::fmod(currentFrameIndex, 1);
        }
        return std::make_shared<VideoSimulationFrame>(frames[0]);
    }

    // check existing frames and convert new ones as needed
    if (const size_t frameCount = frames.size(); index >= frameCount) {
        for (int i = 0; i < index - frameCount + 1; ++i) {
            if (!convertNextVideoFrame()) {
                currentFrameIndex = 0;
                index = 0;
                break;
            }
        }
    }

    // should only occur if no frames are present in buffer
    if (index >= frames.size())
        return nullptr;

    return std::make_shared<VideoSimulationFrame>(frames[index]);
}

bool VideoSimulation::isContinuous() {
    return isCam;
}

bool VideoSimulation::captureOpened() const {
    return capture.isOpened();
}

bool VideoSimulation::convertNextVideoFrame() {
    cv::Mat frame;

    if (!capture.read(frame))
        return false;

    VideoMatrix m(simulationHeight, simulationWidth);
    const double dx = static_cast<double>(frame.cols) / simulationWidth;
    const double dy = static_cast<double>(frame.rows) / simulationHeight;
    for (int y = 0; y < simulationHeight; y++) {
        for (int x = 0; x < simulationWidth; x++) {
            m(y, x) = frame.at<cv::Vec3b>(static_cast<int>(y * dy), static_cast<int>(x * dx));
        }
    }

    frames.push_back(VideoSimulationFrame(m));
    return true;
}
