#include "QSynthi2/Simulation/SimThread.h"
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
    Decimal timestep;
    Decimal speed;

    while (!terminate) {
        if (started) {
            // update parameters
            if (newParameters) {
                std::lock_guard lock(parameterMutex);
                timestep = this->timestep;
                speed = this->speed;
                newParameters = false;
            }

            appendFrame(new SimFrame(sim->getNextFrame(0.2, {})));
        } else {
            // todo try both busy waiting and sleep with new buffer method
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

void SimThread::updateParameters(const ParameterCollection* pc, const List<ModulationData>& md) {
    std::lock_guard lock(parameterMutex);
    newParameters = true;
}

void SimThread::appendFrame(SimFrame* f) {
    std::lock_guard lock(frameMutex);
    newestFrame = frameBuffer.append(f);
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
