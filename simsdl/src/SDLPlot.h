#ifndef SDLPLOT_H
#define SDLPLOT_H
#include "SDL2/SDL.h"
#include <vector>
#include <complex>

class SDLPlot
{
public:
	SDL_Window* window = nullptr;
	SDL_Surface* surface = nullptr;
	SDL_Renderer* renderer = nullptr;

	// SDL Event handler
	SDL_Event e;

	const int width;
	const int height;

	int simW{}, simH{};

	bool quit;

	SDLPlot(const int width, const int height)
		: e()
		, width(width)
		, height(height) {
		// SDL boilerplate
		SDL_Init(SDL_INIT_VIDEO);
		window = SDL_CreateWindow("2D Plot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		                          width, height,
		                          SDL_WINDOW_SHOWN);
		surface = SDL_GetWindowSurface(window);
		renderer = SDL_CreateRenderer(window, -1, 0);

		quit = false;
	}

	~SDLPlot()
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		// quit SDL subsystems
		SDL_Quit();
	}

	void draw2DField(const CList& values) {
		draw2DField(values.map<num>([](const cnum c){ return abs(c); }));
	}

	void draw2DField(const RList& values) {
		const num w = simW;
		const num h = simH;

		const num vx = width / w;
		const num vy = height / h;

		// clear background
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);


		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				auto r = SDL_FRect(x * vx, y * vy, vx, vy);
				// determine color
				// const double re = values.at(x * w + y).real();
				// const double im = values.at(x * w + y).imag();
				int rgb = std::min(255, static_cast<int>(std::round(std::pow(std::abs(values.at(x * w + y)), 0.66) * 255)));
				// int v_re = std::max(0, std::min(255, static_cast<int>(round(re * 255))));
				// int v_im = std::max(0, std::min(255, static_cast<int>(round(im * 255))));
				SDL_SetRenderDrawColor(renderer, rgb, rgb, rgb, SDL_ALPHA_OPAQUE);
				//SDL_SetRenderDrawColor(renderer, re, 0, im, SDL_ALPHA_OPAQUE);
				// fill rectangle
				SDL_RenderFillRectF(renderer, &r);
			}
		}

		// update screen
		SDL_RenderPresent(renderer);
	}
};

#endif //SDLPLOT_H
