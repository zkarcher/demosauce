#ifndef GREEN_CYCLE_H__
#define GREEN_CYCLE_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"

const uint_fast8_t GREEN_STROKE = 50;

uint_fast8_t colorPhase = 0;
uint8_t colorLookup[128];

uint_fast8_t drawPhase = 0;

uint_fast16_t lineCount = 0;

void greenCycle_setup( ILI9341_t3 tft ) {
  //uint_fast16_t w = tft.width();
  //uint_fast16_t h = tft.height();
}

void greenCycle_perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  uint_fast16_t w = tft.width();
  uint_fast16_t h = tft.height();

  // Advance the lookup table
  colorPhase++;
  for( uint_fast8_t c=0; c<32; c++ ) {
    colorLookup[c] = colorPhase % 64;
    colorLookup[32+c*2] = colorLookup[33+c*2] = (colorPhase + 17) % 64;
    colorLookup[96+c] = (colorPhase + 37) % 64;
  }
  tft.setColorSet( colorLookup );

	// Draw stuff
	uint_fast8_t y0 = random( 0, h-GREEN_STROKE );
	uint_fast8_t y1 = random( 0, h-GREEN_STROKE );
	uint_fast16_t clr = tft.color565( 0, drawPhase, 0 );

	if( lineCount > 200 ) {
		y0 = y1 = 0;
		clr = 0x0;
	}

	for( uint_fast8_t i=0; i<GREEN_STROKE; i++ ) {
		tft.drawLine( 0, y0+i, w-1, y1+i, clr );
	}

	lineCount++;

	drawPhase++;
}

#endif
