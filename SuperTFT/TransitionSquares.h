#ifndef TRANSITION_SQUARES_H__
#define TRANSITION_SQUARES_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"

struct TransitionSquaresVars {
  float _phase;
	uint_fast16_t _color;
};
TransitionSquaresVars tsq = (TransitionSquaresVars){ 0 };

void transitionSquares_reset( ILI9341_t3 tft, uint_fast16_t inColor ) {
  //uint_fast16_t w = tft.width();
  //uint_fast16_t h = tft.height();
	tsq._phase = 0;
	tsq._color = inColor;
}

// Return true when complete
boolean transitionSquares_perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  uint_fast16_t w = (uint_fast16_t)tft.width();
  uint_fast16_t h = (uint_fast16_t)tft.height();

	tsq._phase += frameParams.timeMult * 0.01;

	const uint_fast8_t SQ_SIZE = 40;
	const uint_fast8_t size = SQ_SIZE * min( tsq._phase, 1.0f );

	// Draw squares, offset so squares grow out from center
	for( uint_fast16_t x=((SQ_SIZE-size)>>1); x<w; x+=SQ_SIZE ) {
		for( uint_fast16_t y=((SQ_SIZE-size)>>1); y<h; y+=SQ_SIZE ) {
			tft.fillRect( x, y, size, size, tsq._color );
		}
	}

	return (tsq._phase >= 1.0f);
}

#endif
