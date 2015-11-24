#ifndef PLASMA_H__
#define PLASMA_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"

const float PLASMA_SPEED = 0.02;
const uint_fast8_t PLASMA_DITHER = 8;
const uint_fast16_t PLASMA_MARGIN = 25;

struct PlasmaVars {
  float _phase;
	//uint_fast8_t _ditherX;
	uint_fast8_t _ditherY;
  uint_fast16_t _bgColor;
};
PlasmaVars plasma = (PlasmaVars){ 0 };

void plasma_setup( ILI9341_t3 tft ) {
  //uint_fast16_t w = tft.width();
  //uint_fast16_t h = tft.height();

  plasma._bgColor = tft.color565( 0xff, 0xff, 0 );
}

uint_fast16_t plasma_bgColor(){
	return plasma._bgColor;
}

void plasma_perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  int_fast16_t w = (int_fast16_t)tft.width();
  int_fast16_t h = (int_fast16_t)tft.height();

	plasma._phase += frameParams.timeMult * PLASMA_SPEED;

	plasma._ditherY = (plasma._ditherY + 1) % PLASMA_DITHER;

	Point16 p0 = (Point16){
		(int_fast16_t)(w/2 + (sin(plasma._phase*0.57f)*(w/2-PLASMA_MARGIN) )),
		(int_fast16_t)(h/2 + (sin(plasma._phase*0.23f)*(h/2-PLASMA_MARGIN) ))
	};
	Point16 p1 = (Point16){
		(int_fast16_t)(w/2 + (cos(plasma._phase*0.78f)*(w/2-PLASMA_MARGIN) )),
		(int_fast16_t)(h/2 + (cos(plasma._phase*0.42f)*(h/2-PLASMA_MARGIN) ))
	};

	float audioPower = frameParams.audioMean;

	for( int_fast16_t x=0; x<w; x+=PLASMA_DITHER ) {
		for( int_fast16_t y=plasma._ditherY; y<h; y+=PLASMA_DITHER ) {
			Point16 d0 = (Point16){ p0.x - x, p0.y - y };
			Point16 d1 = (Point16){ p1.x - x, p1.y - y };

			//uint_fast16_t distance = sqrt( d0.x*d0.x + d0.y*d0.y );// * sqrt( d1.x*d1.x + d1.y*d1.y );	// SLOW
			//uint_fast16_t distance = abs( d0.x*d0.y );	// pretty good
			uint_fast16_t distance = abs( d0.x*d0.y + d1.x*d1.y );

			uint_fast8_t bright = lerp8( (uint_fast16_t)(distance >> 4) & 0xff, 0xff, audioPower );

			uint_fast16_t color = tft.color565( bright, bright, 0 );

			tft.drawFastHLine( x, y, PLASMA_DITHER, color );
		}
	}
}

#endif
