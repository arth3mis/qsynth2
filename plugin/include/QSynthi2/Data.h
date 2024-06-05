#ifndef DATA_H
#define DATA_H
#include <mutex>
#include <atomic>
#include "QSynthi2/Parameter/ParameterCollection.h"

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

    // thread-safe
    //
    ParameterCollection* parameters;

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
