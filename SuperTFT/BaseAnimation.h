#ifndef BASE_ANIMATION_H__
#define BASE_ANIMATION_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"

class BaseAnimation {
public:
	BaseAnimation(){};

	virtual void init( ILI9341_t3 tft );

	virtual uint_fast16_t bgColor( void );
	virtual void reset( ILI9341_t3 tft );
	virtual void perFrame( ILI9341_t3 tft, FrameParams frameParams );
	virtual boolean canTransition( void );
};

/*
void BaseAnimation::init( ILI9341_t3 tft ) {
	// Extend me
}

uint_fast16_t BaseAnimation::bgColor( void ) {
	// Extend me
	return 0xf81f;	// Everyone loves magenta
}

void BaseAnimation::reset( ILI9341_t3 tft ) {
	// Extend me
}

void BaseAnimation::perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
	// Extend me
}

boolean BaseAnimation::canTransition( void ) {
	// Extend me

	return true;		// Default: Animations can be changed at any time
}
*/

#endif
