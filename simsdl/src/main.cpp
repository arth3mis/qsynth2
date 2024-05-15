#include <chrono>
#include <iostream>
#include <QSynthi2/Simulation/QuantumSimulation.h>
#include "QSynthi2/Data.h"

#include "SDLPlot.h"

int main() {
	SDLPlot sdl(700, 700);

	// field
	constexpr int N = 128;
	constexpr int w = N;
	constexpr int h = N;
	sdl.simW = w;
	sdl.simH = h;

	auto sim = QuantumSimulation(w, h)
		// .barrierPotential({0, NAN}, 2, {{-0.1f, 0.1f}}, 1e30)
		.barrierPotential({-0.0, NAN}, 2, {{-0.2, -0.1}, {0.1, 0.2}}, 1e30)
		.parabolaPotential({0, 0}, {2, 1.5})
		.gaussianDistribution({-0.4, 0}, {0.25, 0.25}, {4, 0});

	const num dt = 0.2;
	const ModulationData mod;

	bool running = true, isReset = false;
	int initDraw = 2;

	size_t simCount = 0;
	namespace cr = std::chrono;
	const auto t = cr::system_clock::now();

	// draw loop
	while (!sdl.quit) {
		while (SDL_PollEvent(&sdl.e) != 0) {
			if (sdl.e.type == SDL_QUIT || sdl.e.type == SDL_KEYDOWN && sdl.e.key.keysym.sym == SDLK_ESCAPE) { sdl.quit = true; }
			else if (sdl.e.type == SDL_KEYDOWN && sdl.e.key.keysym.sym == SDLK_BACKSPACE) {
				sim.reset();
				isReset = true;
			}
			else if (sdl.e.type == SDL_KEYDOWN && sdl.e.key.keysym.sym == SDLK_SPACE) {
				running = !running;
			}
			else if (sdl.e.type == SDL_KEYDOWN && sdl.e.key.keysym.sym == SDLK_RIGHT) {
				sdl.clearField();
				sdl.draw2DField(sim.getNextFrame(dt, mod), 0xffffff, {1, 0});
				sdl.update();
			}
		}

		// draw
		if (running || isReset || initDraw > 0) {
			sdl.clearField();
			sdl.draw2DField(sim.getPsi(), 0xffff00, {1, 0});
			sdl.draw2DField(sim.getPotentials()[0], 0x0000ff, {1e-30, 0});
			sdl.update();
			isReset = false;
			initDraw--;
		}

		// calculate
		constexpr int reps = 20;
		for (size_t i = 0; i < reps && running; i++) {
			sim.getNextFrame(dt, mod);
		}

		if (running) {
			// log
			simCount += reps;
			if (simCount % 1000 == 0
				|| simCount == 5*reps
				|| simCount == 25*reps) {
				std::cout
					<< simCount << " sim steps in "
					<< cr::duration_cast<cr::milliseconds>(cr::system_clock::now() - t).count() << " ms\n";
			}
		}

		// delay?
#define LIMIT_RATE 0
#define TICK_RATE 50

#if LIMIT_RATE
		SDL_Delay(TICK_RATE);
#endif
	}
}

