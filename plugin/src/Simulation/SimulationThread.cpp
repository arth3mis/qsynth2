#include "QSynthi2/Simulation/SimulationThread.h"
#include <QSynthi2/Parameter/ParameterCollection.h>
#include <QSynthi2/Data.h>

extern Data sharedData;


SimulationThread::SimulationThread(const std::shared_ptr<Simulation> &s) {
    simulation = s;
    newSimulation = nullptr;
    newestFrame = -1;
    started = false;
    terminate = false;
    reset = false;
    t = std::thread(&SimulationThread::simulationLoop, this);
}

SimulationThread::~SimulationThread() {
    t.join();
}

void SimulationThread::simulationLoop() {
    // parameters
    Decimal timestep = 0;
    newParameters = true;

    while (!terminate) {
        // update simulation
        if (newSimulation) {
            std::lock_guard lock(simulationMutex);
            simulation = newSimulation;
            newSimulation = nullptr;
            newParameters = true;
        }
        // update parameters
        if (newParameters) {
            std::lock_guard lock(parameterMutex);
            timestep = this->timestep;
            newParameters = false;
        }

        if (frameBuffer.size() < bufferTargetSize) {
            if (reset) {
                reset = false;
                simulation->reset();
                std::lock_guard lock(frameMutex);
                frameBuffer.clear();
            }
            if (!started || juce::approximatelyEqual(timestep, static_cast<Decimal>(0))) {
                if (frameBuffer.empty())
                    appendFrame(simulation->getStartFrame());
                else
                    appendFrame(std::shared_ptr<SimulationFrame>(frameBuffer.back()->clone()));
            }
            appendFrame(simulation->getNextFrame(timestep, {}));
        }
        // else: busy wait
    }
}

void SimulationThread::updateParameters(const ParameterCollection* parameterCollection, const List<ModulationData*> &modulationDataList) {
    std::lock_guard lock(parameterMutex);
    newParameters = true;

    Decimal simulationStepsPerSecond = parameterCollection->simulationStepsPerSecond->getSingleModulated(modulationDataList);
    if (sharedData.videoFps > 0) {
        simulationStepsPerSecond = sharedData.videoFps;
    }
    const Decimal simulationSpeedFactor = parameterCollection->simulationSpeedFactor->getSingleModulated(modulationDataList);
    const Decimal simulationBufferSeconds = parameterCollection->simulationBufferSeconds->getSingleModulated(modulationDataList);

    bufferTargetSize = std::max(static_cast<size_t>(round(simulationBufferSeconds * simulationStepsPerSecond)), static_cast<size_t>(2));
    timestep = simulationSpeedFactor / simulationStepsPerSecond;

    // simulation parameters
    simulation->updateParameters(parameterCollection, modulationDataList);
}

void SimulationThread::appendFrame(const SimulationFramePointer& f) {
    if (f == nullptr)
        return;
    std::lock_guard lock(frameMutex);
    frameBuffer.append(f);
    ++newestFrame;
}

FrameList SimulationThread::getFrames(const size_t n) {
    std::lock_guard lock(frameMutex);
    jassert(bufferTargetSize >= 2 * n); // buffer must at least double the size of requested frames, so the simulation thread can write in one half while the audio thread reads the other half

    const auto first = frameBuffer.begin();
    const auto last = std::next(first, static_cast<long>(std::min(n, frameBuffer.size())));
    auto subList = FrameList(first, last);
    frameBuffer.erase(first, last);
    return subList;
}

SimulationFramePointer SimulationThread::getStartFrame() {
    std::lock_guard lock(simulationMutex);
    return simulation->getStartFrame();
}

size_t SimulationThread::frameReadyCount() {
    std::lock_guard lock(frameMutex);
    return frameBuffer.size();
}

bool SimulationThread::isSimulationContinuous() {
    std::lock_guard lock(simulationMutex);
    return simulation->isContinuous();
}

void SimulationThread::resetSimulation() {
    reset = true;
}

void SimulationThread::resetSimulation(const std::shared_ptr<Simulation> &s) {
    newSimulation = s;
}
