#include "QSynthi2/Simulation/VideoSimulation.h"

VideoSimulation::VideoSimulation(const int targetWidth, const int targetHeight, const juce::String& filename) {
    file = filename;
    simulationWidth = targetWidth;
    simulationHeight = targetHeight;
    currentFrameIndex = 0;

    // init video capture
    capture.open(file.toStdString());
    if(!capture.isOpened()) {
        juce::Logger::writeToLog("Video file could not be opened: " + file);
    }
}

VideoSimulation::~VideoSimulation() = default;

void VideoSimulation::reset() {
    currentFrameIndex = 0;
}

SimulationFramePointer VideoSimulation::getNextFrame(const Decimal timestep, const ModulationData &modulationData) {
    if (!capture.isOpened())
        return nullptr;

    currentFrameIndex += timestep;
    auto index = static_cast<size_t>(currentFrameIndex);

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
