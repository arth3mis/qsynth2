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

    std::atomic<size_t> simWidth;
    std::atomic<size_t> simHeight;
    std::atomic<long> scanlineY{0}; // todo remove

    // thread-safe
    //
    ParameterCollection* parameters;

    // context: audio thread
    // TODO: find different word for index
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
