#ifndef SIMTHREAD_H
#define SIMTHREAD_H

#include "QSynthi2/types.h"
#include "QSynthi2/Simulation/Simulation.h"
#include <thread>
#include <mutex>
#include <memory>
#include <QSynthi2/Parameter/ParameterCollection.h>

class SimThread {
public:

    explicit SimThread(const std::shared_ptr<Simulation>& s);
    ~SimThread();

    void simulationLoop();
    void updateParameters(const ParameterCollection* pc, const List<ModulationData>& md);
    void appendFrame(SimFrame* f);
    std::vector<std::shared_ptr<SimFrame>> getFrames(size_t n);
    size_t frameReadyCount();

    std::atomic<bool> started;
    std::atomic<bool> terminate;

    std::atomic<long> newestFrame;

private:
    std::thread t;
    std::shared_ptr<Simulation> sim;

    // parameters
    std::mutex parameterMutex;
    std::atomic<bool> newParameters;
    Decimal timestep = 0;
    Decimal speed = 0;

    // frames
    std::mutex frameMutex;
    std::atomic<size_t> bufferTargetSize;
    List<SimFrame*> frameBuffer;
};

#endif //SIMTHREAD_H
