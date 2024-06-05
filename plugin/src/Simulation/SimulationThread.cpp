#include "QSynthi2/Simulation/SimulationThread.h"
#include <QSynthi2/Parameter/ParameterCollection.h>
#include <chrono>

SimulationThread::SimulationThread(const std::shared_ptr<Simulation> &s) {
    sim = s;
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
            appendFrame(std::make_shared<SimulationFrame>(sim->getNextFrame(timestep, {})));
        } else {
            // todo try both busy waiting and sleep with new buffer method
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
}

void SimulationThread::updateParameters(const ParameterCollection* pc, const List<ModulationData>& md) {
    std::lock_guard lock(parameterMutex);
    newParameters = true;

    // TODO read from parameter
    bufferTargetSize = 50;
}

void SimulationThread::appendFrame(const std::shared_ptr<SimulationFrame>& f) {
    std::lock_guard lock(frameMutex);
    frameBuffer.append(f);
    ++newestFrame;
}

std::vector<std::shared_ptr<SimulationFrame>> SimulationThread::getFrames(const size_t n) {
    std::lock_guard lock(frameMutex);
    jassert(frameBuffer.size() >= 2 * n); // buffer must at least double the size of requested frames, so the simulation thread can write in one half while the audio thread reads the other half

    const auto first = frameBuffer.begin();
    const auto last = std::next(first, static_cast<long>(std::min(n, frameBuffer.size())));
    auto subList = List<std::shared_ptr<SimulationFrame>>(first, last);
    frameBuffer.erase(first, last);
    return subList;
}

size_t SimulationThread::frameReadyCount() {
    // TODO: use atomic for size?
    std::lock_guard lock(frameMutex);
    return frameBuffer.size();
}
