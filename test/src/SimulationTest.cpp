#include <gtest/gtest.h>
#include <QSynthi2/Simulation/QuantumSimulation.h>

namespace audio_plugin_test {

    void print2D(const List<float>& l, const QuantumSimulation& sim) {
        const std::wstring c = L".,;*?$▓";
        for (int i = 0; i < sim.getH(); ++i) {
            for (int j = 0; j < sim.getW(); ++j) {
                const wchar_t ch = c[static_cast<int>(round(
                    std::max(0.f, std::min(1.f, l[j*sim.getH()+i])) * c.length()))];
                std::wcout << ch << ch << ch;
            }
            std::cout << std::endl;
        }
            std::cout << std::endl;
    }

    TEST(QuantumSimulation, Setup) {
        auto sim = QuantumSimulation(16, 16, 0.01)
            .parabolaPotential({}, {1, 1})
            .gaussianDistribution({-0.4, 0}, {0.2, 0.2}, {0, 0});
        const auto p = sim.getPsi();
        //p[2][3] = {5};

        // std::cout << p.map<float>([](const cnum c){ return round(abs(c)*10000)/10000.f; });
        print2D(p->map<float>([](const cnum c){ return abs(c); }), sim);

        sim.getNextFrame();
        // sim.getNextFrame();

        print2D(p->map<float>([](const cnum c){ return abs(c); }), sim);
    }

} // namespace audio_plugin_test