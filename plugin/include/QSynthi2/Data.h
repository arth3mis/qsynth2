#ifndef DATA_H
#define DATA_H
#include <mutex>
#include <atomic>
#include "QSynthi2/Parameter/ParameterCollection.h"
#include "QSynthi2/Testing/Stopwatch.h"

class Data {
public:

    SimFrame getSimulationDisplayFrame();
    SimFrame* getSimulationScanFrame();
    void setSimulationDisplayFrame(const SimFrame& f);
    void setSimulationScanFrame(const SimFrame& f);

    std::atomic<size_t> simWidth;
    std::atomic<size_t> simHeight;
    std::atomic<long> scanlineY{0};

    ParameterCollection parameters;

    Stopwatch functionCallStopwatch     {"Function calls"};
    Stopwatch parameterStopwatch        {"Parameters    "};
    Stopwatch modulationStopwatch       {"Modulation    "};
    Stopwatch hashMapStopwatch          {"Read Hashmap  "};
    Stopwatch blockStopwatch            {"Block         "};
    Stopwatch totalStopwatch            {"total sim     "};

private:

    SimFrame simulationDisplayFrame;
    SimFrame simulationScanFrame;
    std::mutex frameAccessMutex;
};

#endif //DATA_H
