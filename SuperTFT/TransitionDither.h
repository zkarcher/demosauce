#ifndef TRANSITION_DITHER_H__
#define TRANSITION_DITHER_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"
#include "BaseTransition.h"


const float TRANSITION_DITHER_SPEED = 0.02f;
const uint_fast8_t MSB_BITS = 4;
const uint_fast8_t LSB_BITS = 4;


class TransitionDither : public BaseTransition {
public:
	TransitionDither() : BaseTransition() {};

  void init( ILI9341_t3 tft );
	void restart( ILI9341_t3 tft, uint_fast16_t color );
	void perFrame( ILI9341_t3 tft, FrameParams frameParams );
  boolean isComplete();

private:
	float _phase = 0;
	uint_fast16_t _color;
	uint_fast8_t _msb;
	uint_fast8_t _lsb;
};

void TransitionDither::init( ILI9341_t3 tft ) {

}

void TransitionDither::restart( ILI9341_t3 tft, uint_fast16_t inColor ) {
  //uint_fast16_t w = tft.width();
  //uint_fast16_t h = tft.height();
	_phase = 0;
	_color = inColor;
	_msb = 0;
	_lsb = 0;
}

void TransitionDither::perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  uint_fast16_t w = (uint_fast16_t)tft.width();
  uint_fast16_t h = (uint_fast16_t)tft.height();

	_phase += frameParams.timeMult * TRANSITION_DITHER_SPEED;

	// Calculate destination _msb and _lsb
	float msbPhase_f = min(_phase,0.99999f) * (float)(1<<MSB_BITS);
	uint_fast8_t msbTarget = floor(msbPhase_f);

	float lsbPhase_f = (msbPhase_f - (float)msbTarget) * (float)(1<<LSB_BITS);
	uint_fast8_t lsbTarget = floor(lsbPhase_f);

	// Draw dither dots until _msb and _lsb hit their targets.
	while( (_msb != msbTarget) || (_lsb != lsbTarget) ) {

		

		for( uint_fast16_t i=_lsb; i<w; i+=(1<<LSB_BITS) ) {
			for( uint_fast16_t j=_msb; j<h; j+=(1<<MSB_BITS) ) {
				tft.drawPixel( i, j, _color );
			}
		}

		_lsb++;

		if( _lsb == (1<<LSB_BITS) ) {
			_lsb = 0;
			_msb++;
		}

	}

}

boolean TransitionDither::isComplete() {
  return _phase >= 1.0f;
}

#endif
