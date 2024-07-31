#include "QSynthi2/Simulation/SimulationThread.h"
#include <QSynthi2/Parameter/ParameterCollection.h>

SimulationThread::SimulationThread(const std::shared_ptr<Simulation> &s) {
    simulation = s;
    timestamp = 0;
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
    while (!terminate) {
        // cut history buffer
        if (historyBuffer.size() > historySize) {
            historyBuffer.erase(
                historyBuffer.begin(),
                historyBuffer.begin() + static_cast<long>(historyBuffer.size() - historySize));
        }
        // fill buffer
        if (frameBuffer.size() < bufferTargetSize) {
            // reset simulation
            if (reset) {
                simulation->reset();
                historyBuffer.clear();
                timestamp = 0;
                std::lock_guard lock(frameMutex);
                frameBuffer.clear();
                reset = false;
            }
            // append frame buffer
            const Decimal timestep = this->timestep;
            long historyIndex = -2;  // -2: not used; -1: no history available; >=0: can go backward to here

            if (started) {
                timestamp += timestep;

                // setup for moving backward
                if (timestep < 0 && !juce::approximatelyEqual(timestep, static_cast<Decimal>(0))) {
                    // find frame directly before requested timestamp
                    for (historyIndex = static_cast<long>(historyBuffer.size() - 1); historyIndex >= 0; --historyIndex) {
                        if (historyBuffer[static_cast<size_t>(historyIndex)]->timestamp <= timestamp) {
                            break;
                        }
                    }
                }
            }

            // stationary frame
            if (!started || historyIndex == -1 || juce::approximatelyEqual(timestep, static_cast<Decimal>(0))) {
                if (frameBuffer.empty())
                    appendFrame(simulation->getStartFrame());
                else
                    appendFrame(std::shared_ptr<SimulationFrame>(frameBuffer.back()->clone()));
            }
            // frame from history
            else if (timestep < 0) {
                const size_t h = static_cast<size_t>(historyIndex);
                // remove later frames as they won't be used anymore
                historyBuffer.erase(
                    historyBuffer.begin() + (historyIndex + 1),
                    historyBuffer.end());
                // overwrite simulation with old frame
                simulation->setState(historyBuffer[h]);
                if (juce::approximatelyEqual(historyBuffer[h]->timestamp, timestamp)) {
                    // use already existent frame
                    appendFrame(historyBuffer[h]);
                } else {
                    // simulate until exact requested time
                    appendFrame(simulation->getNextFrame(timestamp - historyBuffer[h]->timestamp));
                }
            }
            // new frame
            else {
                auto frame = simulation->getNextFrame(timestep);
                appendFrame(frame);
                historyBuffer.append(frame);
            }
        }
        // else: busy wait
    }
}

void SimulationThread::updateParameters(const ParameterCollection* parameterCollection, const List<ModulationData*> &modulationDataList) {
    const Decimal simulationStepsPerSecond = parameterCollection->simulationStepsPerSecond->getSingleModulated(modulationDataList);
    const Decimal simulationSpeedFactor = parameterCollection->simulationSpeedFactor->getSingleModulated(modulationDataList);
    const Decimal simulationBufferSeconds = parameterCollection->simulationBufferSeconds->getSingleModulated(modulationDataList);
    const Decimal simulationHistorySeconds = parameterCollection->simulationHistorySeconds->getSingleModulated(modulationDataList);

    bufferTargetSize = std::max(static_cast<size_t>(round(simulationBufferSeconds * simulationStepsPerSecond)), static_cast<size_t>(2));
    historySize = static_cast<size_t>(simulationHistorySeconds * simulationStepsPerSecond);
    timestep = simulationSpeedFactor / simulationStepsPerSecond;

    // simulation parameters
    simulation->updateParameters(parameterCollection, modulationDataList);
}

void SimulationThread::appendFrame(const SimulationFramePointer& f) {
    if (f == nullptr)
        return;
    f->timestamp = timestamp;
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
    return simulation->getStartFrame();
}

size_t SimulationThread::frameReadyCount() {
    std::lock_guard lock(frameMutex);
    return frameBuffer.size();
}

bool SimulationThread::isSimulationContinuous() {
    return simulation->isContinuous();
}

void SimulationThread::resetSimulation() {
    reset = true;
}
