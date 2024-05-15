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

	void clearField() const {
		// clear background
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
	}

	void update() const {
		// update screen
		SDL_RenderPresent(renderer);
	}

	void draw2DField(const CList& values, const int rgbMask, const V2 valueMod) {
		draw2DField(values.map<num>([](const cnum c){ return abs(c); }), rgbMask, valueMod);
	}

	void draw2DField(const RList& values, const int rgbMask, const V2 valueMod) {
		const num w = simW;
		const num h = simH;

		const num vx = width / w;
		const num vy = height / h;

		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				auto r = SDL_FRect(x * vx, y * vy, vx, vy);
				// determine color
				// const double re = values.at(x * w + y).real();
				// const double im = values.at(x * w + y).imag();
				num v = values.at(x * w + y) * valueMod.x + valueMod.y;
				int rgb = std::min(255, static_cast<int>(std::round(std::pow(std::abs(v), 0.66) * 255)));
				if (rgb < 2)
					continue;
				// int v_re = std::max(0, std::min(255, static_cast<int>(round(re * 255))));
				// int v_im = std::max(0, std::min(255, static_cast<int>(round(im * 255))));
				int cR = rgb * ((rgbMask & 0xff0000) >> 16) / 255;
				int cG = rgb * ((rgbMask & 0x00ff00) >> 8) / 255;
				int cB = rgb * (rgbMask & 0x0000ff) / 255;
				SDL_SetRenderDrawColor(renderer, cR, cG, cB, SDL_ALPHA_OPAQUE);
				//SDL_SetRenderDrawColor(renderer, re, 0, im, SDL_ALPHA_OPAQUE);
				// fill rectangle
				SDL_RenderFillRectF(renderer, &r);
			}
		}
	}
};

#endif //SDLPLOT_H
