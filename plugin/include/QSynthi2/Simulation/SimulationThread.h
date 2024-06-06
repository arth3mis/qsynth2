#ifndef SIMTHREAD_H
#define SIMTHREAD_H

#include "QSynthi2/types.h"
#include "QSynthi2/Simulation/Simulation.h"
#include <thread>
#include <mutex>
#include <memory>

class ParameterCollection;

class SimulationThread {
public:

    explicit SimulationThread(const std::shared_ptr<Simulation>& s);
    ~SimulationThread();

    void simulationLoop();
    void updateParameters(const ParameterCollection* parameterCollection, const List<ModulationData*>& modulationData);
    void appendFrame(const std::shared_ptr<SimulationFrame>& f);
    FrameList getFrames(size_t n);
    size_t frameReadyCount();

    void resetSimulation() const;

    std::atomic<bool> started;
    std::atomic<bool> terminate;

    std::atomic<long> newestFrame;
    size_t sleepCounter = 0;

private:
    std::thread t;
    std::shared_ptr<Simulation> simulation;

    // parameters
    std::mutex parameterMutex;
    std::atomic<bool> newParameters;
    Decimal timestep = 0.2;

    // frames
    std::mutex frameMutex;
    std::atomic<size_t> bufferTargetSize;
    FrameList frameBuffer;
};

#endif //SIMTHREAD_H
