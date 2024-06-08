#include "QSynthi2/Data.h"

SimulationFrame Data::getSimulationDisplayFrame() {
    std::lock_guard lock(frameAccessMutex);
    return simulationDisplayFrame;
}

SimulationFrame* Data::getSimulationScanFrame() {
    return &simulationScanFrame;
}

void Data::setSimulationDisplayFrame(const SimulationFrame& f) {
    std::lock_guard lock(frameAccessMutex);
    simulationDisplayFrame = f;
}

void Data::setSimulationScanFrame(const SimulationFrame& f) {
    simulationScanFrame = f;
}

void Data::appendFrameBuffer(const FrameList &frames) {
    if (!frames.empty()) {
        // append data
        frameBuffer.append(frames);
        // update display frame
        setSimulationDisplayFrame(*frames.back());
    }
}
