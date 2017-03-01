#ifndef HEADER_GUARD_BOOP_BEEPER_RENDERER_GRAPHIC_HPP_INCLUDED
#define HEADER_GUARD_BOOP_BEEPER_RENDERER_GRAPHIC_HPP_INCLUDED

#include "../SDL_support/MySDL_Render.hpp"

using namespace VaExc;

namespace morse_graphic_renderer
{
	// Rendering constants:
	const size_t TILE_SIDE = 100;
	const size_t TILES_X_COUNT = 8;
	const size_t TILES_Y_COUNT = 6;
	const size_t SCREEN_W = TILES_X_COUNT * TILE_SIDE;
	const size_t SCREEN_H = TILES_Y_COUNT * TILE_SIDE;
	const size_t MORSE_SIDE = TILE_SIDE / 10;

	// MySDL::Renderer wrapper
	class MorseRenderer
	{
	private:
		SDL_Window* window_;
		MySDL::Renderer renderer_;

	public:
		MorseRenderer() :
			window_ (SDL_CreateWindow("SDL_RENDERER", 50, 50, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN)),
			renderer_ (window_)
		{
			if (window_ == nullptr)
			{
				throw Exception(ArgMsg("Main: Can not create a window, %s", SDL_GetError()));
			}

			renderer_.setLineColor({255, 0, 255, 255});
			renderer_.setFillColor({  0, 0,   0,   0});
		}

		~MorseRenderer()
		{
			SDL_DestroyWindow(window_);
		}

		MySDL::Renderer& getRenderer() { return renderer_; }

	};

	void MorseGraphicsInit()
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			throw Exception("Can't initialize SDL"_msg, VAEXC_POS);
		}
	}

	void MorseGraphicsQuit()
	{
		SDL_Quit();
	}

	void MorseGraphicsRender(MorseSymbol morseSymbol)
	{
		// MorseRender to render with
		static MorseRenderer renderer{};

		// A queue to store previous morse symbols
		static VaQueue::Queue<MorseSymbol, TILES_X_COUNT * TILES_Y_COUNT> lastElements{};

		if (lastElements.size() == lastElements.capasity())
		{
			lastElements.pop_front();
		}

		if (morseSymbol != '_') lastElements.push_back(morseSymbol);

		// Rendering queue:

		renderer.getRenderer().startRendering();
		renderer.getRenderer().clear({0, 0, 0, 0});

		for (size_t i = 0; i < lastElements.size(); ++i)
		{
			int curX = (i % TILES_X_COUNT) * TILE_SIDE + TILE_SIDE / 2;
			int curY = (i / TILES_X_COUNT) * TILE_SIDE + TILE_SIDE / 2;

			switch (lastElements.at(i))
			{
				case '.':
				{
					renderer.getRenderer().circle(curX, curY, TILE_SIDE/10);
					break;
				}
				case '-':
				{
					// Drawing rectangle
					renderer.getRenderer().line(curX - 3 * MORSE_SIDE, curY - MORSE_SIDE,
					                            curX - 3 * MORSE_SIDE, curY + MORSE_SIDE);
					renderer.getRenderer().line(curX + 3 * MORSE_SIDE, curY - MORSE_SIDE,
					                            curX + 3 * MORSE_SIDE, curY + MORSE_SIDE);
					renderer.getRenderer().line(curX - 3 * MORSE_SIDE, curY - MORSE_SIDE,
					                            curX + 3 * MORSE_SIDE, curY - MORSE_SIDE);
					renderer.getRenderer().line(curX - 3 * MORSE_SIDE, curY + MORSE_SIDE,
					                            curX + 3 * MORSE_SIDE, curY + MORSE_SIDE);
					break;
				}
				case ' ':
				{
					// Drawing underscore
					renderer.getRenderer().line(curX - 3 * MORSE_SIDE, curY + 3 * MORSE_SIDE,
					                            curX + 3 * MORSE_SIDE, curY + 3 * MORSE_SIDE);
					break;
				}
				case '<':
				{
					break;
				}
				case '!':
				{
					renderer.getRenderer().line(curX - 3 * MORSE_SIDE, curY - 3 * MORSE_SIDE,
					                            curX + 3 * MORSE_SIDE, curY + 3 * MORSE_SIDE);
					renderer.getRenderer().line(curX - 3 * MORSE_SIDE, curY + 3 * MORSE_SIDE,
					                            curX + 3 * MORSE_SIDE, curY - 3 * MORSE_SIDE);
					break;
				}
				default:
				{
					throw Exception(ArgMsg("Invalid morse symbol: (%c)", lastElements.at(i)), VAEXC_POS);
				}
			}
		}

		renderer.getRenderer().finishRendering();
		renderer.getRenderer().flash();

		// Waiting after rendering:
		switch (morseSymbol)
		{
			case '.': return SDL_Delay(1 * MORSE_TIME_UNIT);
			case '-': return SDL_Delay(3 * MORSE_TIME_UNIT);
			case '_': return SDL_Delay(1 * MORSE_TIME_UNIT);
			case ' ': return SDL_Delay(3 * MORSE_TIME_UNIT);
			case '<': return SDL_Delay(7 * MORSE_TIME_UNIT);
			case '!': return SDL_Delay(3 * MORSE_TIME_UNIT);
			default:  return;
		}
	}

}  // namespace morse_graphic_renderer

using morse_graphic_renderer::MorseGraphicsRender;
using morse_graphic_renderer::MorseGraphicsInit  ;
using morse_graphic_renderer::MorseGraphicsQuit  ;

#endif  // HEADER_GUARD_BOOP_BEEPER_RENDERER_GRAPHIC_HPP_INCLUDED