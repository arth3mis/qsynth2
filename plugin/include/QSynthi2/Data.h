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
    std::atomic<size_t> simulationWidth;
    std::atomic<size_t> simulationHeight;
    std::atomic<bool> resetSimulation{false};
    juce::String newSimulation = "";

    SimulationFramePointer getSimulationDisplayFrame();

    // thread-safe
    //
    ParameterCollection* parameters;
    List<std::shared_ptr<VoiceData>> voiceData{};

    // context: audio thread
    //
    FrameList frameBuffer;
    size_t frameBufferFirstFrame = 0;
    Eigen::ArrayX<Decimal> frameBufferTimestamps;
    Decimal videoFps = 0;

    void appendFrameBuffer(const FrameList& frames);

private:

    // context: audio thread <-> GUI thread
    //
    SimulationFramePointer simulationDisplayFrame;
    std::mutex frameAccessMutex;
};

#endif //DATA_H
