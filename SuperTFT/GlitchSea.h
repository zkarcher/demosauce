#ifndef GLITCH_SEA_H__
#define GLITCH_SEA_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"
#include "BaseAnimation.h"


class GlitchSea : public BaseAnimation {
public:
	GlitchSea() : BaseAnimation() {};

	void init( ILI9341_t3 tft );
	void reset( ILI9341_t3 tft );
	uint_fast16_t bgColor( void );
	void perFrame( ILI9341_t3 tft, FrameParams frameParams );

private:
	uint16_t * _pixels;
  uint_fast16_t _bgColor;
	float _phase_f = 0;
};

void GlitchSea::init( ILI9341_t3 tft ) {
	_pixels = (uint16_t*)malloc( sizeof(uint16_t) * tft.width() );
  _bgColor = tft.color565( 0x44, 0x44, 0x44 );
}

void GlitchSea::reset( ILI9341_t3 tft ) {
	uint_fast16_t w = tft.width();
	for( uint_fast16_t p=0; p<w; p++ ) {
		_pixels[p] = _bgColor;;
	}
}

uint_fast16_t GlitchSea::bgColor(){
	return _bgColor;
}

void GlitchSea::perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  int_fast16_t w = (int_fast16_t)tft.width();
  int_fast16_t h = (int_fast16_t)tft.height();

	_phase_f += 0.01f * frameParams.timeMult;

	int_fast16_t MAX_WRITE_W = 1;//(w>>3);

	// Discolor a random pixel
	//_pixels[ random(MAX_WRITE_W) ] = tft.color565( random(0xff), random(0xff), random(0xff) );

	// Draw stripes or whatnot
	uint_fast8_t r = random(0xff) | 0x80;
	uint_fast8_t g = random(0xff);
	uint_fast8_t b = random(0xff);
	uint_fast16_t px_x = random( MAX_WRITE_W );
	while( r | g | b ) {
		_pixels[ px_x++ ] = tft.color565( r, g, b );
		r >>= 1;
		g >>= 1;
		b >>= 1;
	}

	int_fast16_t phaseModW = (int_fast16_t)(_phase_f*50.0f) % w;

	// Draw diagonals or whatever
	for( int_fast16_t y=0; y<h; y+=2 ) {

		int_fast16_t offsetX = (y>>1) + phaseModW;
		//offsetX += ( (_phase+y) % 32 );
		//offsetX += ( (_phase-y) ^ 37 ) & 0xff;
		offsetX += sin( y*0.07f + _phase_f ) * 28.0f;
		offsetX = (offsetX + w) % w;

		// Left of offsetX
		/*
		if( offsetX > 0 ) {
			tft.writeRect( 0, y, offsetX, 1, &_pixels[w-offsetX] );
			tft.writeRect( 0, y+1, offsetX, 1, &_pixels[w-offsetX] );
		}
		*/
		// Right of offsetX
		tft.writeRect( offsetX, y, min( w-offsetX, MAX_WRITE_W ), 1, _pixels );
		tft.writeRect( offsetX, y+1, min( w-offsetX, MAX_WRITE_W ), 1, _pixels );
	}

}

#endif
