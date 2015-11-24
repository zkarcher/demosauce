#ifndef TWISTY_TEXT_H__
#define TWISTY_TEXT_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"

const uint_fast8_t PROGMEM TEXT_TABLE[]  = {
	0b11111111,
	0b11000011,
	0b11000011,
	0b01111110,
	0,
	0,
	0b01111110,
	0b11000011,
	0b11000011,
	0b01111110,
	0,
	0,
	0b11111111,
	0b00110011,
	0b00110011,
	0b11101110,
	0,
	0,
	0b11111111,
	0b00001100,
	0b00110110,
	0b11000011,
	0,
	0,
	0b11111111,
	0b11011011,
	0b11011011,
	0b01110110,
	0,
	0,
	0b01111110,
	0b11000011,
	0b11000011,
	0b01111110,
	0,
	0,
	0b00000011,
	0b00000011,
	0b11111111,
	0b00000011,
	0b00000011
};
uint_fast8_t TEXT_TABLE_LENGTH = sizeof( TEXT_TABLE ) / sizeof( uint_fast8_t );

const uint_fast8_t TEXT_PIXEL_WIDTH = 7;
const float TEXT_PIXEL_HEIGHT = 18.0f;
const float TEXT_3D_THICKNESS_MULT = 2.0f;

const float TEXT_SPIN_SPEED = 0.007f;
const float TWIST_AMOUNT = 0.1f;

// This function (which actually gets 'inlined' anywhere it's called)
// exists so that gammaTable can reside out of the way down here in the
// utility code...didn't want that huge table distracting or intimidating
// folks before even getting into the real substance of the program, and
// the compiler permits forward references to functions but not data.

struct TwistyTextVars {
  float _phase;
	uint_fast16_t _bgColor;
};
TwistyTextVars tt = (TwistyTextVars){ 0 };

void twistyText_setup( ILI9341_t3 tft ) {
  //uint_fast16_t w = tft.width();
  //uint_fast16_t h = tft.height();

	tt._bgColor = tft.color565( 0, 0, 0x66 );
}

uint_fast16_t twistyText_bgColor(){
	return tt._bgColor;
}

void twistyText_perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  //uint_fast16_t w = (uint_fast16_t)tft.width();
  uint_fast16_t h = (uint_fast16_t)tft.height();

	uint_fast16_t h_2 = (h>>1);

	tt._phase += frameParams.timeMult * TEXT_SPIN_SPEED;
	if( tt._phase > 1.0f ) tt._phase -= 1.0f;

	for( uint_fast8_t c=0; c<TEXT_TABLE_LENGTH; c++ ) {
		uint_fast8_t colByte = pgm_read_byte(&TEXT_TABLE[c]);
		if( colByte == 0 ) continue;	// Premature optimization: Skip empty columns

		uint_fast16_t left = c * TEXT_PIXEL_WIDTH;

		// Erase the background
		uint_fast16_t eraseTop = h_2 - 4.5*TEXT_PIXEL_HEIGHT;

		float angle = (tt._phase * M_PI) + c*TWIST_AMOUNT;

		uint_fast8_t wrapAmt = floor( angle / M_PI );
		angle -= wrapAmt * M_PI;

		float cosAngle = cos( angle );
		float cosAngleAbs = abs( cosAngle );
		float cosAngleNorm = (cosAngle+1.0)*0.5f;
		float sinAngle = sin( angle );

		uint_fast16_t color = tft.color565(
			lerp8( 0x33, 0x88, cosAngleNorm ),
			lerp8( 0x44, 0xff, cosAngleNorm ),
			lerp8( 0x44, 0xbb, cosAngleNorm )
		);

		uint_fast16_t sideColor = tft.color565( 0xff * cosAngleAbs, 0x88 * cosAngleAbs, 0 );

		uint_fast16_t sideHeight = abs( cosAngle ) * TEXT_PIXEL_HEIGHT * TEXT_3D_THICKNESS_MULT;

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

				uint_fast16_t top = h_2 + ((int_fast8_t)(topBit-4) * TEXT_PIXEL_HEIGHT)*sinAngle - (TEXT_PIXEL_HEIGHT*cosAngle);
				uint_fast16_t height = ((bit-topBit) * TEXT_PIXEL_HEIGHT) * sinAngle;

				// Draw the pixel column
				tft.fillRect( left, top, TEXT_PIXEL_WIDTH, height, color );

				// Draw the "side" hanging below, or above, depending on the twist angle
				if( cosAngle > 0.0f ) {
					// Side protudes below: (text is facing up)
					tft.fillRect( left, top+height, TEXT_PIXEL_WIDTH, sideHeight, sideColor );

					// Erase above
					tft.fillRect( left, eraseTop, TEXT_PIXEL_WIDTH, top-eraseTop, tt._bgColor );
					eraseTop = top + height + sideHeight;

				} else {
					// Side protrudes above: (text is facing down)
					tft.fillRect( left, top-sideHeight, TEXT_PIXEL_WIDTH, sideHeight, sideColor );

					// Erase above
					tft.fillRect( left, eraseTop, TEXT_PIXEL_WIDTH, (top-sideHeight)-eraseTop, tt._bgColor );
					eraseTop = top + height;
				}
			}
		}

		// Erase old graphics below the column
		tft.fillRect( left, eraseTop, TEXT_PIXEL_WIDTH, (h_2+4.5*TEXT_PIXEL_HEIGHT)-eraseTop, tt._bgColor );

	}	// end each column

}

#endif
