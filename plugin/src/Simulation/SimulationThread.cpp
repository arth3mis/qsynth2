#include "QSynthi2/Simulation/SimulationThread.h"
#include <QSynthi2/Parameter/ParameterCollection.h>
#include <chrono>

SimulationThread::SimulationThread(const std::shared_ptr<Simulation> &s) {
    simulation = s;
    newestFrame = -1;
    started = false;
    terminate = false;
    t = std::thread(&SimulationThread::simulationLoop, this);
}

SimulationThread::~SimulationThread() {
    t.join();
}

void SimulationThread::simulationLoop() {
    // parameters
    Decimal timestep = 0;
    Decimal speed = 0;
    newParameters = true;

    while (!terminate) {
        // update parameters
        if (newParameters) {
            std::lock_guard lock(parameterMutex);
            timestep = this->timestep;
            speed = this->speed;
            newParameters = false;
        }

        if (started && frameBuffer.size() < bufferTargetSize) {
            appendFrame(std::make_shared<SimulationFrame>(simulation->getNextFrame(timestep, {})));
        } else {
            // todo try both busy waiting and sleep with new buffer method -> busy waiting seems to work
            // ++sleepCounter;
            // std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

void SimulationThread::updateParameters(const ParameterCollection* pc, const List<ModulationData>& md) {
    std::lock_guard lock(parameterMutex);
    newParameters = true;

    // TODO read from parameter
    bufferTargetSize = 8;
}

void SimulationThread::appendFrame(const std::shared_ptr<SimulationFrame>& f) {
    std::lock_guard lock(frameMutex);
    frameBuffer.append(f);
    ++newestFrame;
}

FrameList SimulationThread::getFrames(const size_t n) {
    std::lock_guard lock(frameMutex);
    jassert(frameBuffer.size() >= 2 * n); // buffer must at least double the size of requested frames, so the simulation thread can write in one half while the audio thread reads the other half

    const auto first = frameBuffer.begin();
    const auto last = std::next(first, static_cast<long>(std::min(n, frameBuffer.size())));
    auto subList = FrameList(first, last);
    frameBuffer.erase(first, last);
    return subList;
}

size_t SimulationThread::frameReadyCount() {
    std::lock_guard lock(frameMutex);
    return frameBuffer.size();
}
