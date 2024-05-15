#ifndef DATA_H
#define DATA_H
#include "QSynthi2/types.h"
#include <mutex>
#include <atomic>
#include "QSynthi2/Parameter/ParameterCollection.h"

typedef List<num> SimFrame;

class Data {
public:
    SimFrame getSimulationDisplayFrame();
    void setSimulationDisplayFrame(const SimFrame& f);
    std::atomic<size_t> simWidth;
    std::atomic<size_t> simHeight;

    ParameterCollection parameters;

private:
    SimFrame simulationDisplayFrame;
    std::mutex frameAccessMutex;
};

#endif //DATA_H
