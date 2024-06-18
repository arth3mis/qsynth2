#include "QSynthi2/Data.h"

SimulationFramePointer Data::getSimulationDisplayFrame() {
    std::lock_guard lock(frameAccessMutex);
    return simulationDisplayFrame;
}

void Data::setSimulationDisplayFrame(const SimulationFramePointer& f) {
    std::lock_guard lock(frameAccessMutex);
    simulationDisplayFrame = f;
}

void Data::appendFrameBuffer(const FrameList &frames) {
    if (!frames.empty()) {
        // append data
        frameBuffer.append(frames);
        // update display frame
        std::lock_guard lock(frameAccessMutex);
        simulationDisplayFrame = frames.back();
    }
}
