#ifndef MY_SDL_KEYBOARD_HPP_INCLUDED
#define MY_SDL_KEYBOARD_HPP_INCLUDED


//----------------------------------------------------------------------------
//{ Includes
//----------------------------------------------------------------------------

	#include <string>

	#include <SDL2/SDL.h>

//}
//----------------------------------------------------------------------------

namespace MySDL
{

//----------------------------------------------------------------------------
//{ Includes
//----------------------------------------------------------------------------

	bool IsPressed(Uint32 scancode)
	{
		static int length = 0;
		static const Uint8* states = SDL_GetKeyboardState(&length);

		if (scancode > static_cast<Uint32>(length))
		{
			throw std::invalid_argument(std::string("IsPressed(): (Uint32) scancode is too big"));
		}

		SDL_PumpEvents();

		return states[scancode];
	}

//}
//----------------------------------------------------------------------------

}

#endif /*MY_SDL_KEYBOARD_HPP_INCLUDED*/