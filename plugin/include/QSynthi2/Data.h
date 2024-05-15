//
// Created by art on 15.05.24.
//

#ifndef DATA_H
#define DATA_H
#include "QSynthi2/types.h"
#include <mutex>
#include <atomic>

typedef List<num> SimFrame;

class Data {
public:
    SimFrame getSimulationDisplayFrame();
    void setSimulationDisplayFrame(const SimFrame& f);
    std::atomic<size_t> simWidth;
    std::atomic<size_t> simHeight;

private:
    SimFrame simulationDisplayFrame;
    std::mutex frameAccessMutex;
};

#endif //DATA_H
