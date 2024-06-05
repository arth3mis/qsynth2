#ifndef DATA_H
#define DATA_H
#include <mutex>
#include <atomic>
#include "QSynthi2/Parameter/ParameterCollection.h"
#include "QSynthi2/Testing/Stopwatch.h"

class Data {
public:

    // context: audio thread <-> GUI thread
    //
    SimulationFrame getSimulationDisplayFrame();
    SimulationFrame* getSimulationScanFrame();
    void setSimulationDisplayFrame(const SimulationFrame& f);
    void setSimulationScanFrame(const SimulationFrame& f);

    std::atomic<size_t> simulationWidth;
    std::atomic<size_t> simulationHeight;

    // thread-safe
    //
    ParameterCollection* parameters;

    // context: audio thread
    Eigen::ArrayX<Decimal> frameBufferTimestamps;
    size_t frameBufferFirstFrame = 0;
    List<std::shared_ptr<SimulationFrame>> frameBuffer;

    Stopwatch totalStopwatch            {"total sim     "};

private:

    // context: audio thread <-> GUI thread
    //
    SimulationFrame simulationDisplayFrame;
    SimulationFrame simulationScanFrame;
    std::mutex frameAccessMutex;
};

#endif //DATA_H
