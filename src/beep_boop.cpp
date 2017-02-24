#include <iostream>
#include <cctype>
#include <cstdlib>
#include <thread>
#include <cstdlib>

#include "queue/Queue.hpp"
#include "queue/VaException.hpp"

#include "Morse.hpp"

using namespace VaExc;

// ToDo: ASCII renderer (change one line of code to switch renderer)

// Rendering morse:

const size_t TILE_SIDE = 100;
const size_t TILES_X_COUNT = 8;
const size_t TILES_Y_COUNT = 6;
const size_t SCREEN_W = TILES_X_COUNT * TILE_SIDE;
const size_t SCREEN_H = TILES_Y_COUNT * TILE_SIDE;
const size_t MORSE_SIDE = TILE_SIDE / 10;

void renderMorse(MorseSymbol morseSymbol, MySDL::Renderer& renderer)
{
	// A queue to store previous morse symbols
	static VaQueue::Queue<MorseSymbol, TILES_X_COUNT * TILES_Y_COUNT> lastElements{};

	// Adding morseSymbol to lastElements

	if (lastElements.size() == lastElements.capasity())
	{
		lastElements.pop_front();
	}

	if (morseSymbol != '_') lastElements.push_back(morseSymbol);

	// Rendering queue:

	renderer.startRendering();
	renderer.clear({0, 0, 0, 0});

	for (size_t i = 0; i < lastElements.size(); ++i)
	{
		int curX = (i % TILES_X_COUNT) * TILE_SIDE + TILE_SIDE / 2;
		int curY = (i / TILES_X_COUNT) * TILE_SIDE + TILE_SIDE / 2;

		switch (lastElements.at(i))
		{
			case '.':
			{
				renderer.circle(curX, curY, TILE_SIDE/10);
				break;
			}
			case '-':
			{
				renderer.line(curX - 3 * MORSE_SIDE, curY - MORSE_SIDE,
				              curX - 3 * MORSE_SIDE, curY + MORSE_SIDE);
				renderer.line(curX + 3 * MORSE_SIDE, curY - MORSE_SIDE,
				              curX + 3 * MORSE_SIDE, curY + MORSE_SIDE);
				renderer.line(curX - 3 * MORSE_SIDE, curY - MORSE_SIDE,
				              curX + 3 * MORSE_SIDE, curY - MORSE_SIDE);
				renderer.line(curX - 3 * MORSE_SIDE, curY + MORSE_SIDE,
				              curX + 3 * MORSE_SIDE, curY + MORSE_SIDE);
				break;
			}
			case ' ':
			{
				renderer.line(curX - 3 * MORSE_SIDE, curY + 3 * MORSE_SIDE,
				              curX + 3 * MORSE_SIDE, curY + 3 * MORSE_SIDE);
				break;
			}
			case '<':
			{
				break;
			}
			case '!':
			{
				renderer.line(curX - 3 * MORSE_SIDE, curY - 3 * MORSE_SIDE,
				              curX + 3 * MORSE_SIDE, curY + 3 * MORSE_SIDE);
				renderer.line(curX - 3 * MORSE_SIDE, curY + 3 * MORSE_SIDE,
				              curX + 3 * MORSE_SIDE, curY - 3 * MORSE_SIDE);
				break;
			}
			default:
			{
				throw Exception(ArgMsg("Invalid morse symbol: (%c)", lastElements.at(i)), VAEXC_POS);
			}
		}
	}

	renderer.finishRendering();
	renderer.flash();

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

// Threads:

std::mutex queue_mutex;

void threadOut(VaQueue::Queue<char, 100>& queue)
{
	try
	{
		// SDL_Window init
		SDL_Window* window = SDL_CreateWindow("SDL_RENDERER", 50, 50, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);
		if (window == nullptr)
		{
			throw Exception(VaExc::ArgMsg("Main: Can not create a window, %s", SDL_GetError()));
		}

		// SDL_Render init
		MySDL::Renderer renderer{window};

		renderer.setLineColor({255, 0, 255, 255});
		renderer.setFillColor({  0, 0,   0,   0});

		// Main cycle:
		bool previousWasSentenceSpace = true;

		while (queue.size() != 0)
		{
			char curChar = 0;

			// Critical section:
			{
				std::lock_guard<std::mutex> lock{queue_mutex};

				curChar = queue.pop_front();
			}

			// Normal section:
			auto curMorseCode = morseFromChar(curChar);

			if (!previousWasSentenceSpace && curMorseCode[0] != '<') renderMorse(' ', renderer);

			for (size_t i = 0; curMorseCode[i] != '\0'; ++i)
			{
				if (curMorseCode[i] == '<') previousWasSentenceSpace = true;
				else previousWasSentenceSpace = false;

				renderMorse(curMorseCode[i], renderer);

				if (curMorseCode[i + 1] != '\0') renderMorse('_', renderer);
			}
		}

		SDL_DestroyWindow(window);
	}
	catch (Exception& exc)
	{
		std::cout << exc.what() << std::endl;
	}
	catch (std::exception& exc)
	{
		std::cout << exc.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unexpected exception" << std::endl;
	}

}

void threadIn(VaQueue::Queue<char, 100>& queue, const std::thread& thr1)
{
	char input = 0;

	do
	{
		input = std::getchar();

		// Critical section:
		std::lock_guard<std::mutex> lock{queue_mutex};

		queue.push_back(input);
	}
	while
	(
		!MySDL::IsPressed(SDL_Scancode::SDL_SCANCODE_Q) &&
		input != '\0' &&
		thr1.joinable()
	);
}

// Main:

MorseCode START_CODE = "eee eee !!! !!! !!!";

int main()
{
	try
	{
		// SDL init
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			using namespace VaExc;

			throw Exception("Can't initialize SDL"_msg, VAEXC_POS);
		}

		// Queue init
		VaQueue::Queue<char, 100> queue{};

		for (size_t i = 0; START_CODE[i] != '\0'; ++i) queue.push_back(START_CODE[i]);

		// Thread init:
		std::thread thr1{threadOut, std::ref(queue)};

		threadIn(queue, thr1);

		thr1.join();

		SDL_Quit();
	}
	catch (Exception& exc)
	{
		std::cout << exc.what() << std::endl;
	}
	catch (std::exception& exc)
	{
		std::cout << exc.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unexpected exception" << std::endl;
	}

	return 0;
}