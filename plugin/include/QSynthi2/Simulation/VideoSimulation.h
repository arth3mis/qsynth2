#ifndef VIDEOSIMULATION_H
#define VIDEOSIMULATION_H

#include "QSynthi2/Simulation/Simulation.h"
#include "QSynthi2/Video.h"

typedef Eigen::ArrayXX<cv::Vec3b> VideoMatrix;

class VideoSimulationFrame final : public SimulationFrame {
public:
    explicit VideoSimulationFrame(const VideoMatrix& m) {
        frame = m;
    }
    [[nodiscard]] SimulationFrame* clone() override {
        return new VideoSimulationFrame(frame);
    }
    [[nodiscard]] RealMatrix toDecimal() const override {
        // TODO implement if needed
        return {};
    }
    [[nodiscard]] RealMatrix toPhase() const override {
        // TODO implement if needed
        return {};
    }
    [[nodiscard]] Decimal toDecimal(long row, long col) const override;

    [[nodiscard]] Decimal toPhase(const long row, const long col) const override {
        return frame(row, col)[1] / 255.0;
    }
    [[nodiscard]] size_t cols() const override {
        return frame.cols();
    }
    [[nodiscard]] size_t rows() const override {
        return frame.rows();
    }
private:
    VideoMatrix frame;
};

class VideoSimulation : public Simulation {
public:

    VideoSimulation(int targetWidth, int targetHeight, const juce::String& filename);
    ~VideoSimulation() override;

    void reset() override;

    // getters
    SimulationFramePointer getNextFrame(Decimal timestep, const ModulationData& modulationData) override;

    bool captureOpened() const;

private:

    int simulationWidth;
    int simulationHeight;
    juce::String file;
    cv::VideoCapture capture;
    List<VideoSimulationFrame> frames;
    Decimal currentFrameIndex;

    bool convertNextVideoFrame();
};



#endif //VIDEOSIMULATION_H
