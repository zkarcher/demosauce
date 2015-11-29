#ifndef TRANSITION_SCROLL_H__
#define TRANSITION_SCROLL_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"
#include "BaseTransition.h"


const float TRANSITION_SCROLL_SPEED = 0.002f;
const float SCROLL_WRAPS = 5.0f;


class TransitionScroll : public BaseTransition {
public:
	TransitionScroll() : BaseTransition() {};

  void init( ILI9341_t3 tft );
	void restart( ILI9341_t3 tft, uint_fast16_t color );
	void perFrame( ILI9341_t3 tft, FrameParams frameParams );
  boolean isComplete();

private:
	float _phase = 0;
	uint_fast16_t _color;
	uint_fast16_t _lastScroll;
};

void TransitionScroll::init( ILI9341_t3 tft ) {

}

void TransitionScroll::restart( ILI9341_t3 tft, uint_fast16_t inColor ) {
  //uint_fast16_t w = tft.width();
  //uint_fast16_t h = tft.height();
	_phase = 0;
	_color = inColor;
	_lastScroll = tft.width();
}

float easeInOutCubic( float p ) {
  if( p < 0.5f ) {
    float p2 = p * 2.0f;
    return (p2*p2*p2) * 0.5f;
  }

  float p2 = (1.0f - p) * 2.0f;
  return 1.0f - (p2*p2*p2) * 0.5f;
}

void TransitionScroll::perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  uint_fast16_t w = (uint_fast16_t)tft.width();
  uint_fast16_t h = (uint_fast16_t)tft.height();

	_phase += frameParams.timeMult * TRANSITION_SCROLL_SPEED * SCROLL_WRAPS;

	// Apply some easing. Linear speed feels dull
	float ease = min( easeInOutCubic( _phase ), 1.0f );

	// Hmm. Let's try reading pixel color values and drawing a weird smear effect?
	/*
	float lineX_f = w * (1.0f-easeOutQuad);
	uint_fast16_t lineX = floor(lineX_f);
	uint_fast16_t lineW = min( (w - lineX) - 1, 64 );
	*/

	float unwrapped = SCROLL_WRAPS * ease;
	float wrap = unwrapped - floor(unwrapped);

	float scroll_f = w * (1.0f - wrap);
	uint_fast16_t scroll = floor(scroll_f);

	tft.setScroll( scroll );

	// Final frame: Fill with destination color
	if( unwrapped > (SCROLL_WRAPS-1) ) {
		tft.fillRect( (w-_lastScroll), 0, _lastScroll - scroll, h, _color );
		_lastScroll = scroll;
	}

}

boolean TransitionScroll::isComplete() {
  return _phase >= 1.0f;
}

#endif
