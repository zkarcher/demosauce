#ifndef TWISTY_TEXT_H__
#define TWISTY_TEXT_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"

const uint_fast8_t PROGMEM TEXT_TABLE[]  = {
	0b11111111,
	0b00011000,
	0b00011000,
	0b11111111,
	0b0,
	0b0,
	0b11111111,
	0b11011011,
	0b11011011,
	0b11011011,
	0b0,
	0b0,
	0b00000111,
	0b00011100,
	0b11110000,
	0b00011100,
	0b00000111
};
const uint_fast8_t TEXT_TABLE_LENGTH = 17;

const uint_fast8_t TEXT_PIXEL_WIDTH = 10;
const float TEXT_PIXEL_HEIGHT = 10.0f;

const float TWIST_AMOUNT = 0.1f;

// This function (which actually gets 'inlined' anywhere it's called)
// exists so that gammaTable can reside out of the way down here in the
// utility code...didn't want that huge table distracting or intimidating
// folks before even getting into the real substance of the program, and
// the compiler permits forward references to functions but not data.

struct TwistyTextVars {
  float _phase;
};
TwistyTextVars tt = (TwistyTextVars){ 0 };

void twistyText_setup( ILI9341_t3 tft ) {
  //uint_fast16_t w = tft.width();
  //uint_fast16_t h = tft.height();
}

void twistyText_perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  //uint_fast16_t w = (uint_fast16_t)tft.width();
  uint_fast16_t h = (uint_fast16_t)tft.height();

	uint_fast16_t h_2 = (h>>1);

	tt._phase += frameParams.timeMult * 0.007;
	if( tt._phase > 1.0f ) tt._phase -= 1.0f;

	for( uint_fast8_t c=0; c<TEXT_TABLE_LENGTH; c++ ) {
		uint_fast16_t left = c * TEXT_PIXEL_WIDTH;

		// Erase the background
		tft.fillRect( left, h_2 - 4*TEXT_PIXEL_HEIGHT, TEXT_PIXEL_WIDTH, 8*TEXT_PIXEL_HEIGHT, 0x0 );

		float angle = (tt._phase * M_PI) + c*TWIST_AMOUNT;
		if( angle > M_PI ) angle -= M_PI;

		float cosAngle = cos( angle );
		float cosAngleNorm = (cosAngle+1.0)*0.5f;
		float sinAngle = sin( angle );

		uint_fast16_t color = tft.color565(
			lerp8( 0x33, 0x88, cosAngleNorm ),
			lerp8( 0x44, 0xff, cosAngleNorm ),
			lerp8( 0x44, 0xbb, cosAngleNorm )
		);

		uint_fast8_t colByte = pgm_read_byte(&TEXT_TABLE[c]);

		// Draw a minimal number of rects. Advance from top to bottom. Track when rects start & end.
		boolean inRect = false;
		uint_fast8_t topBit;
		for( uint_fast8_t bit=0; bit<=8; bit++ ) {

			boolean isSolid = (boolean)( colByte & (0x1 << bit) );

			if( !inRect && isSolid ) {
				inRect = true;
				topBit = bit;

			} else if( inRect && !isSolid ) {
				inRect = false;

				uint_fast16_t top = h_2 + ((int_fast8_t)(topBit-4) * TEXT_PIXEL_HEIGHT) * sinAngle;
				uint_fast16_t height = ((bit-topBit) * TEXT_PIXEL_HEIGHT) * sinAngle;

				// Draw the pixels
				tft.fillRect( left, top, TEXT_PIXEL_WIDTH, height, color );
			}
		}
	}
}

#endif
