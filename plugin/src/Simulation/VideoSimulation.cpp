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
    currentFrameIndex = 0;

    // init video capture
    isCam = false;
    isSingleFrame = false;
    if (file == "0") {
        isCam = true;
        capture.open(0);
    } else if (!cv::imread(file.toStdString()).empty()) {
        isSingleFrame = true;
        capture.open(file.toStdString());
    } else {
        capture.open(file.toStdString());
    }

    if (!capture.isOpened()) {
        juce::Logger::writeToLog("Video file could not be opened: " + file);
    }

    simulationWidth = targetWidth;
    simulationHeight = targetHeight;
    if (targetWidth == -1) {
        simulationWidth = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    }
    if (targetHeight == -1) {
        simulationHeight = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    }
}

VideoSimulation::VideoSimulation(const VideoSimulation &s)
    : VideoSimulation(s.simulationWidth, s.simulationHeight, s.file) {
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
        if (!convertNextVideoFrame())
            return nullptr;

    sharedData.barrierX = -1.01;

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

    if (isSingleFrame) {
        if (frames.empty())
            if (!convertNextVideoFrame())
                return nullptr;
        currentFrameIndex = 0;
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

bool VideoSimulation::isStationary() {
    return isSingleFrame;
}

bool VideoSimulation::captureOpened() const {
    return capture.isOpened();
}

int VideoSimulation::videoFps() const {
    return static_cast<int>(capture.get(cv::CAP_PROP_FPS));
}

bool VideoSimulation::convertNextVideoFrame() {
    std::lock_guard lock(captureMutex);
    cv::Mat frame;

    if (!capture.read(frame))
        return false;

    cv::Mat resizedFrame;
    cv::resize(frame, resizedFrame, cv::Size(simulationWidth, simulationHeight), 0, 0,
        cv::INTER_LINEAR);

    VideoMatrix m(simulationHeight, simulationWidth);
    for (int y = 0; y < simulationHeight; y++) {
        for (int x = 0; x < simulationWidth; x++) {
            m(y, x) = resizedFrame.at<cv::Vec3b>(y, x);
        }
    }

    frames.push_back(VideoSimulationFrame(m));
    return true;
}
