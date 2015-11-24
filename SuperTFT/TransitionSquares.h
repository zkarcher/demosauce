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

const float TRANSITION_SQUARES_SPEED = 0.06;
const float TRANSITION_SQUARES_SLOPE = 0.05f;

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

	tsq._phase += frameParams.timeMult * TRANSITION_SQUARES_SPEED;

	const uint_fast8_t SQ_SIZE = 30;
  boolean anySmallSquares = false;

	// Draw squares, offset so squares grow out from center
  uint_fast8_t across = w / SQ_SIZE + 1;
  uint_fast8_t down = h / SQ_SIZE + 1;
  for( uint_fast8_t i=0; i<across; i++ ) {
    for( uint_fast8_t j=0; j<down; j++ ) {

      uint_fast8_t size = SQ_SIZE * constrain( tsq._phase - (i+j)*TRANSITION_SQUARES_SLOPE, 0.0f, 1.0f );
      if( size < SQ_SIZE ) anySmallSquares = true;

      // Squares should grow out from center
      tft.fillRect( (i+0.5f)*SQ_SIZE - (size>>1), (j+0.5f)*SQ_SIZE - (size>>1), size, size, tsq._color );
    }
  }

  // When all squares are full-size, the transition is done!
	return !anySmallSquares;
}

#endif
