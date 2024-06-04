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
    SimFrame getSimulationDisplayFrame();
    SimFrame* getSimulationScanFrame();
    void setSimulationDisplayFrame(const SimFrame& f);
    void setSimulationScanFrame(const SimFrame& f);

    std::atomic<size_t> simWidth;
    std::atomic<size_t> simHeight;
    std::atomic<long> scanlineY{0};

    // context: audio thread
    //
    std::vector<std::shared_ptr<SimFrame>> scannerFrames;

    // thread-safe
    //
    ParameterCollection* parameters;

    // context: audio thread
    // TODO: find different word for index
    Decimal currentSimulationFrameIndex = 0;
    Eigen::ArrayX<Decimal> relativeSimulationFrameIndices = {};
    List<ComplexMatrix> relativeSimulationFrames = {};

    Stopwatch totalStopwatch            {"total sim     "};

private:

    // context: audio thread <-> GUI thread
    //
    SimFrame simulationDisplayFrame;
    SimFrame simulationScanFrame;
    std::mutex frameAccessMutex;
};

#endif //DATA_H
