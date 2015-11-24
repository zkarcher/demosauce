#ifndef TWISTY_TEXT_FONT_H__
#define TWISTY_TEXT_FONT_H__

const uint_fast8_t PROGMEM FONT_TABLE[]  = {
	// A
	0b11111100,
	0b00110110,
	0b00110011,
	0b00110110,
	0b11111100,

	// B
	0b11111111,
	0b11011011,
	0b11011011,
	0b11011011,
	0b01110110,

	// C
	0b01111110,
	0b11000011,
	0b11000011,
	0b11000011,
	0b01100110,

	// D
	0b11111111,
	0b11000011,
	0b11000011,
	0b11000011,
	0b01111110,

	// E
	0b11111111,
	0b11011011,
	0b11011011,
	0b11011011,
	0b11000011,

	// F
	0b11111111,
	0b00011011,
	0b00011011,
	0b00011011,
	0b00000011,

	// G
	0b01111110,
	0b11000011,
	0b11011011,
	0b11011011,
	0b01111010,

	// H
	0b11111111,
	0b00011000,
	0b00011000,
	0b00011000,
	0b11111111,

	// I
	0b00000000,
	0b00000111,
	0b11110101,
	0b00000111,
	0b00000000,

	// J
	0b01100000,
	0b11000000,
	0b11000011,
	0b11000011,
	0b01111111,

	// K
	0b11111111,
	0b00011000,
	0b00111100,
	0b01100110,
	0b11000011,

	// L
	0b11111111,
	0b11000000,
	0b11000000,
	0b11000000,
	0b11000000,

	// M
	0b11111111,
	0b00000110,
	0b11111100,
	0b00000110,
	0b11111111,

	// N
	0b11111111,
	0b00000011,
	0b00000011,
	0b00000011,
	0b11111110,

	// O
	0b01111110,
	0b11000011,
	0b11000011,
	0b11000011,
	0b01111110,

	// P
	0b11111111,
	0b00110011,
	0b00110011,
	0b00110011,
	0b00011110,

	// Q
	0b01111110,
	0b11000011,
	0b11000011,
	0b11110011,
	0b11111110,

	// R
	0b11111111,
	0b00110011,
	0b00110011,
	0b00110011,
	0b11101110,

	// S
	0b11001110,
	0b11011011,
	0b11011011,
	0b11011011,
	0b01110011,

	// T
	0b00000011,
	0b00000011,
	0b11111111,
	0b00000011,
	0b00000011,

	// U
	0b01111111,
	0b11000000,
	0b11000000,
	0b11000000,
	0b01111111,

	// V
	0b00111111,
	0b01100000,
	0b11000000,
	0b01100000,
	0b00111111,

	// W
	0b01111111,
	0b11000000,
	0b01111111,
	0b11000000,
	0b01111111,

	// X
	0b11100011,
	0b00110110,
	0b00011100,
	0b00110110,
	0b11100011,

	// Y
	0b00001111,
	0b00011000,
	0b11110000,
	0b00011000,
	0b00001111,

	// Z
	0b11100011,
	0b11110011,
	0b11011011,
	0b11001111,
	0b11000011,

	// +
	0b00011000,
	0b00011000,
	0b01111110,
	0b00011000,
	0b00011000,

	// :
	0b00000000,
	0b01100110,
	0b01100110,
	0b00000000,
	0b00000000,

	// -
	0b00011000,
	0b00011000,
	0b00011000,
	0b00011000,
	0b00011000,

	// !
	0b00000000,
	0b11011111,
	0b11011111,
	0b00000000,
	0b00000000
};
uint_fast8_t FONT_TABLE_LENGTH = sizeof( FONT_TABLE ) / sizeof( uint_fast8_t );

#endif
