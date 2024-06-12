#ifndef VIDEOSIMULATION_H
#define VIDEOSIMULATION_H

#include "QSynthi2/Simulation/Simulation.h"

class VideoSimulation : public Simulation {
public:

    VideoSimulation(int targetWidth, int targetHeight, const juce::String& filename);
    ~VideoSimulation() override;

    void reset() override;

    // getters
    const ComplexMatrix& getNextFrame(Decimal timestep, const ModulationData& modulationData) override;

private:
    juce::String file;

    ComplexMatrix psi;
    List<RealMatrix> frames;
};



#endif //VIDEOSIMULATION_H
