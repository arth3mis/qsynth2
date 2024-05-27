#include <gtest/gtest.h>
#include <QSynthi2/Simulation/QuantumSimulation.h>

namespace audio_plugin_test {

    void print2D(const RealMatrix& m) {
        const std::wstring c = L".,;*?$";
        for (int i = 0; i < m.rows(); ++i) {
            for (int j = 0; j < m.cols(); ++j) {
                const wchar_t ch = c[static_cast<int>(std::round(
                    std::max(0.0, std::min(1.0, std::pow(m(i, j), 0.66))) * (static_cast<float>(c.length()) - 1.f)))];
                std::wcout << ch << ch << ch;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    TEST(QuantumSimulation, Setup) {
        auto sim = QuantumSimulation(16, 16)
        // .barrierPotential({-0.0, NAN}, 2, {{-0.2, -0.1}, {0.1, 0.2}}, 1e30)
        .parabolaPotential({0, 0}, {20, 20})
        .gaussianDistribution({-0.4, 0}, {0.25, 0.25}, {0, 0});

        print2D(Eigen::abs(sim.getPsi()));

        for (int i = 0; i < 10; ++i) {
            sim.getNextFrame(0.2, ModulationData());
        }

        print2D(Eigen::abs(sim.getPsi()));
    }

} // namespace audio_plugin_test