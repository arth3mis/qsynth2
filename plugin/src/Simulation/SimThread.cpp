#include "QSynthi2/Simulation/SimThread.h"
#include <QSynthi2/Parameter/ParameterCollection.h>
#include <chrono>

SimThread::SimThread(const std::shared_ptr<Simulation> &s) {
    sim = s;
    newestFrame = -1;
    started = false;
    terminate = false;
    t = std::thread(&SimThread::simulationLoop, this);
}

SimThread::~SimThread() {
    t.join();
    frameBuffer.forEach([](const SimFrame* m) { delete m; });
}

void SimThread::simulationLoop() {
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
            appendFrame(new SimFrame(sim->getNextFrame(timestep, {})));
        } else {
            // todo try both busy waiting and sleep with new buffer method
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
}

void SimThread::updateParameters(const ParameterCollection* pc, const List<ModulationData>& md) {
    std::lock_guard lock(parameterMutex);
    newParameters = true;

    // TODO temp
    bufferTargetSize = 50;
}

void SimThread::appendFrame(SimFrame* f) {
    std::lock_guard lock(frameMutex);
    frameBuffer.append(f);
    ++newestFrame;
}

std::vector<std::shared_ptr<SimFrame>> SimThread::getFrames(const size_t n) {
    std::lock_guard lock(frameMutex);
    const auto first = frameBuffer.begin();
    const auto last = std::next(first, static_cast<long>(std::min(n, frameBuffer.size())));
    auto subList = std::vector<std::shared_ptr<SimFrame>>(first, last);
    frameBuffer.erase(first, last);
    return subList;
}

size_t SimThread::frameReadyCount() {
    std::lock_guard lock(frameMutex);
    return frameBuffer.size();
}
