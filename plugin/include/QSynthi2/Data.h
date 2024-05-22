#ifndef DATA_H
#define DATA_H
#include "QSynthi2/types.h"
#include <mutex>
#include <atomic>
#include "QSynthi2/Parameter/ParameterCollection.h"
#include "QSynthi2/Testing/Stopwatch.h"

class Data {
public:

    SimFrame getSimulationDisplayFrame();
    void setSimulationDisplayFrame(const SimFrame& f);
    std::atomic<size_t> simWidth;
    std::atomic<size_t> simHeight;

    ParameterCollection parameters;

    SimFrame *currentFrame;
    std::atomic<size_t> scanlineY{0};

    Stopwatch functionCallStopwatch     {"Function calls"};
    Stopwatch parameterStopwatch        {"Parameters    "};
    Stopwatch modulationStopwatch       {"Modulation    "};
    Stopwatch hashMapStopwatch          {"Read Hash Map "};
    Stopwatch simulationStopwatch       {"Simulation    "};
    Stopwatch simFftStopwatch           {"Sim FFT       "};
    Stopwatch simKinStopwatch           {"Sim Kin       "};
    Stopwatch simPotStopwatch           {"Sim Pot       "};
    Stopwatch blockStopwatch            {"Block         "};
    Stopwatch totalStopwatch            {"total sim     "};

private:

    SimFrame simulationDisplayFrame;
    std::mutex frameAccessMutex;
};

#endif //DATA_H
