#include <iostream>
#include <cctype>
#include <cstdlib>
#include <thread>
#include <cstdlib>

#include "queue/Queue.hpp"
#include "queue/VaException.hpp"

#include "Morse.hpp"

#include "renderers/MorseGraphicRenderer.hpp"
#include "renderers/MorseConsoleRenderer.hpp"

// Threads:

std::mutex queue_mutex;

void threadOut(VaQueue::Queue<char, 100>& queue)
{
	try
	{
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

			if (!previousWasSentenceSpace && curMorseCode[0] != '<') MorseConsoleRender(' ');

			for (size_t i = 0; curMorseCode[i] != '\0'; ++i)
			{
				if (curMorseCode[i] == '<') previousWasSentenceSpace = true;
				else previousWasSentenceSpace = false;

				MorseConsoleRender(curMorseCode[i]);

				if (curMorseCode[i + 1] != '\0') MorseConsoleRender('_');
			}
		}
	}
	catch (VaExc::Exception& exc)
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
		MorseConsoleInit();

		// Queue init
		VaQueue::Queue<char, 100> queue{};

		for (size_t i = 0; START_CODE[i] != '\0'; ++i) queue.push_back(START_CODE[i]);

		// Thread init:
		std::thread thr1{threadOut, std::ref(queue)};

		threadIn(queue, thr1);

		thr1.join();

		MorseConsoleQuit();
	}
	catch (VaExc::Exception& exc)
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