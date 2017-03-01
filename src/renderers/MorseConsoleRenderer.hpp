#ifndef HEADER_GUARD_BOOP_BEEPER_RENDERER_CONSOLE_HPP_INCLUDED
#define HEADER_GUARD_BOOP_BEEPER_RENDERER_CONSOLE_HPP_INCLUDED

#include <cstdio>
#include <chrono>

void MorseConsoleInit()
{
	std::system("stty raw -echo");
}

void MorseConsoleQuit()
{
	std::system("stty cooked");
}

void MorseConsoleRender(MorseSymbol morseSymbol)
{
	switch (morseSymbol)
	{
		case '.':
		{
			std::printf(".");
			std::fflush(stdout);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			break;
		}
		case '-':
		{
			std::printf("-");
			std::fflush(stdout);
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			break;
		}
		case ' ':
		{
			std::printf(" ");
			std::fflush(stdout);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			break;
		}
		case '_':
		{
			break;
		}
		case '<':
		{
			std::printf("\n\r");
			std::fflush(stdout);
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			break;
		}
		case '!':
		{
			std::printf("!");
			std::fflush(stdout);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			break;
		}
		default:
		{
			throw Exception(ArgMsg("Invalid morse symbol: (%c)", morseSymbol), VAEXC_POS);
		}
	}
}

#endif  // HEADER_GUARD_BOOP_BEEPER_RENDERER_CONSOLE_HPP_INCLUDED