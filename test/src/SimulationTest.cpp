#include <gtest/gtest.h>
#include <QSynthi2/Simulation/QuantumSimulation.h>

namespace audio_plugin_test {

    void print2D(const List<float>& l, const QuantumSimulation& sim) {
        const std::wstring c = L".,;*?$";
        for (int i = 0; i < sim.getH(); ++i) {
            for (int j = 0; j < sim.getW(); ++j) {
                const wchar_t ch = c[static_cast<int>(std::round(
                    std::max(0.f, std::min(1.f, l[j*sim.getH()+i])) * (static_cast<float>(c.length()) - 1.f)))];
                std::wcout << ch << ch << ch;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    TEST(QuantumSimulation, Setup) {
        auto sim = QuantumSimulation(16, 16)
            .parabolaPotential({}, {10, 10})
            .gaussianDistribution({-0.5, 0}, {0.2, 0.2}, {0, 0});

        print2D(sim.getPsi().map<float>([](const Complex c){ return abs(c); }), sim);

        for (int i = 0; i < 5; ++i) {
            sim.getNextFrame(0.2f, ModulationData());
        }

        print2D(sim.getPsi().map<float>([](const Complex c){ return abs(c); }), sim);
    }

} // namespace audio_plugin_test