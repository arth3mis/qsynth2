#include <malloc.h>
#include <gtest/gtest.h>
#include <QSynthi2/Data.h>
#include <QSynthi2/Simulation/QuantumSimulation.h>
#include <QSynthi2/Simulation/SimulationThread.h>

namespace audio_plugin_test {

    void print2D(const RealMatrix& m) {
        const std::wstring c = L".,;*#§";
        for (int i = 0; i < m.rows(); ++i) {
            for (int j = 0; j < m.cols(); ++j) {
                const wchar_t ch = c[static_cast<size_t>(std::round(
                    std::max(0.0, std::min(1.0, std::pow(m(i, j), 0.66))) * (static_cast<float>(c.length()) - 1.f)))];
                std::wcout << ch << ch << ch;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    TEST(QuantumSimulation, Setup) {
        auto sim = QuantumSimulation(128, 128)
        // .barrierPotential({-0.0, NAN}, 2, {{-0.2, -0.1}, {0.1, 0.2}}, 1e30)
        .parabolaPotential({0, 0}, {20, 20})
        .gaussianDistribution({-0.4, 0}, {0.25, 0.25}, {0, 0});

        // print2D(Eigen::abs(sim.getPsi()));

        // TODO valgrind --tool=massif ./cmake-build-debug/test/AudioPluginTest
        //      then use Massif-Visualizer

        // TODO check Eigen preprocessor directives for possible speed increase
        //      https://eigen.tuxfamily.org/dox/TopicPreprocessorDirectives.html

        for (int i = 0; i < 10; ++i) {
            sim.getNextFrame(0.2);
        }

        // print2D(Eigen::abs(sim.getPsi()));
    }

    TEST(QuantumSimulation, Thread) {
        auto simulation = std::dynamic_pointer_cast<Simulation>(std::make_shared<QuantumSimulation>(QuantumSimulation(128,128)
        .barrierPotential(BARRIER_VERTICAL, 0, 2, {{-0.2, -0.1}, {0.1, 0.2}}, 1e30)
        .parabolaPotential({0, 0}, {2, 1.5})
        .gaussianDistribution({-0.4, 0}, {0.25, 0.25}, {4, 0})));

        auto simulationThread = new SimulationThread(simulation);

        FrameList frames(100);

        simulationThread->started = true;


        for (int i=0; i<100; i++) {

            size_t neededSimulationFrames = 1;

            if (simulationThread->frameReadyCount() <= neededSimulationFrames) {
                juce::Logger::writeToLog("Busy wait for simulation thread.");
                int busyWaitCounter = 0;
                while (simulationThread->frameReadyCount() <= neededSimulationFrames) {
                    busyWaitCounter++;
                    if (busyWaitCounter >= 100000) {
                        break;
                    }
                }
            }
            // juce::Logger::writeToLog("Simulation thread is " + juce::String(simulationThread->frameReadyCount() - neededSimulationFrames) + " frames ahead.");

            size_t before = mallinfo().uordblks;

            // append the shared frame buffer
            // this also sets the latest simulation frame as the display frame (if a new one arrived)
            auto newFrames = simulationThread->getFrames(neededSimulationFrames);

            frames.append(newFrames);

            size_t after = mallinfo().uordblks;
            std::cout << "Allocated: " << (after - before) << " bytes\n";
        }
    }

    TEST(QuantumSimulation, FrontendPointer) {
        Data sharedData;

        auto sim = QuantumSimulation(128, 128);

        size_t before = mallinfo().uordblks;
        sharedData.setSimulationDisplayFrame(sim.getStartFrame());

        auto f = sharedData.getSimulationDisplayFrame();
        size_t after = mallinfo().uordblks;
        std::cout << "Allocated: " << (after - before) << " bytes\n";
    }

} // namespace audio_plugin_test