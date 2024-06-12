#ifndef DATA_H
#define DATA_H
#include <mutex>
#include <atomic>
#include "QSynthi2/Parameter/ParameterCollection.h"
#include "QSynthi2/Synthesizer/VoiceData.h"

class Data {
public:

    // context: audio thread <-> GUI thread
    //
    SimulationFrame getSimulationDisplayFrame();
    SimulationFrame* getSimulationScanFrame(); // todo remove?
    void setSimulationDisplayFrame(const SimulationFrame& f);
    void setSimulationScanFrame(const SimulationFrame& f); // todo remove?

    std::atomic<size_t> simulationWidth;
    std::atomic<size_t> simulationHeight;

    std::atomic<bool> resetSimulation{false};

    // thread-safe
    //
    ParameterCollection* parameters;
    List<std::shared_ptr<VoiceData>> voiceData{};

    // context: audio thread
    Eigen::ArrayX<Decimal> frameBufferTimestamps;
    size_t frameBufferFirstFrame = 0;
    FrameList frameBuffer;

    void appendFrameBuffer(const FrameList& frames);

private:

    // context: audio thread <-> GUI thread
    //
    SimulationFrame simulationDisplayFrame;
    SimulationFrame simulationScanFrame;
    std::mutex frameAccessMutex;
};

#endif //DATA_H
