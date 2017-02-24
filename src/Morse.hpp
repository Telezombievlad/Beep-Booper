#ifndef HEADER_GUARD_BOOP_BEEPER_MORSE_HPP_INCLUDED
#define HEADER_GUARD_BOOP_BEEPER_MORSE_HPP_INCLUDED

#include "SDL_support/MySDL_Render.hpp"
#include "SDL_support/MySDL_Keyboard.hpp"

// Morse table:
// '.' - dot
// '-' - dash
// '_' - space between dots and dashes in letter
// ' ' - space between leters
// '<' - space between words

using MorseSymbol = char;
using MorseCode   = const char*;

MorseCode MORSE_TABLE[36] =
{
	".-"  , // A
	"-...", // B
	"-.-.", // C
	"-.." , // D
	"."   , // E
	"..-.", // F
	"--." , // G
	"....", // H
	".."  , // I
	".---", // J
	"-.-" , // K
	".-..", // L
	"--"  , // M
	"-."  , // N
	"---" , // O
	".--.", // P
	"--.-", // Q
	".-." , // R
	"..." , // S
	"-"   , // T
	"..-" , // U
	"...-", // V
	".--" , // W
	"-..-", // X
	"-.--", // Y
	"--..", // Z
	"-----", // 0
	".----", // 1
	"..---", // 2
	"...--", // 3
	"....-", // 4
	".....", // 5
	"-....", // 6
	"--...", // 7
	"---..", // 8
	"----."  // 9
};

const size_t MORSE_INDEX_ZERO = 26;

MorseCode MORSE_SPACE_IN_LETTER   = "_";
MorseCode MORSE_SPACE_IN_WORD     = " ";
MorseCode MORSE_SPACE_IN_SENTENCE = "<";
MorseCode MORSE_UNKNOWN           = "!";

// Morse unit of time
const unsigned MORSE_TIME_UNIT = 100; // milliseconds

// Translating to morse:
MorseCode morseFromChar(char toConvert)
{
	if ('a' <= toConvert && toConvert <= 'z') return MORSE_TABLE[toConvert - 'a'];
	if ('A' <= toConvert && toConvert <= 'z') return MORSE_TABLE[toConvert - 'A'];
	if ('0' <= toConvert && toConvert <= '9') return MORSE_TABLE[toConvert - '0' + MORSE_INDEX_ZERO];

	if (std::isspace(toConvert)) return MORSE_SPACE_IN_SENTENCE;

	return MORSE_UNKNOWN;
}

#endif  // HEADER_GUARD_BOOP_BEEPER_MORSE_HPP_INCLUDED