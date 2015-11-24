#ifndef PLASMA_CLOUD_H__
#define PLASMA_CLOUD_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"

const float PLASMA_CLOUD_SPEED = 0.02;
const uint_fast8_t PLASMA_CLOUD_DITHER = 4;
const uint_fast16_t PLASMA_CLOUD_MARGIN = 25;

struct PlasmaCloudVars {
  float _phase;
	uint_fast8_t _ditherX;
	uint_fast8_t _ditherY;
  uint_fast16_t _bgColor;
};
PlasmaCloudVars pc = (PlasmaCloudVars){ 0 };

const uint_fast8_t SQRT_TABLE_LENGTH = 255;
uint_fast8_t sqrtTable[ SQRT_TABLE_LENGTH ];
uint_fast8_t sqrtBitShift;	// Shift distances (integers) this many bits. This will give you a lookup index to get the square root.

void plasmaCloud_setup( ILI9341_t3 tft ) {
	pc._bgColor = tft.color565( 0, 0xff, 0 );

  float w = (float)tft.width();
  float h = (float)tft.height();

	float maxScreenDistance = (w*w) + (h*h);
	uint_fast8_t nextPow2 = ceil( log2f( maxScreenDistance ) );
	float maxTableDistance = pow( 2.0f, nextPow2 );
	sqrtBitShift = nextPow2 - 8;

	float sqrtTableMult = (float)0xff / sqrt(maxScreenDistance);

	for( uint_fast8_t i=0; i<SQRT_TABLE_LENGTH; i++ ) {
		float useDistance = ((float)i / (float)SQRT_TABLE_LENGTH) * maxTableDistance;
		float aSqrt = sqrt( useDistance );
		sqrtTable[i] = (uint_fast8_t)(aSqrt * sqrtTableMult);
		//sqrtTable[i] = random(0xff);
	}
}

uint_fast16_t plasmaCloud_bgColor(){
	return pc._bgColor;
}

void plasmaCloud_perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  uint_fast16_t w = (int_fast16_t)tft.width();
  uint_fast16_t h = (int_fast16_t)tft.height();

	pc._phase += frameParams.timeMult * PLASMA_CLOUD_SPEED;

	//py._ditherY = (py._ditherY + 1) % PLASMA_YELLOW_DITHER;

	PointU16 p0 = (PointU16){
		(uint_fast16_t)(w/2 + (sin(pc._phase*0.57f)*(w/2-PLASMA_CLOUD_MARGIN) )),
		(uint_fast16_t)(h/2 + (sin(pc._phase*0.23f)*(h/2-PLASMA_CLOUD_MARGIN) ))
	};
	/*
	PointU16 p1 = (PointU16){
		(uint_fast16_t)(w/2 + (cos(pc._phase*0.78f)*(w/2-PLASMA_CLOUD_MARGIN) )),
		(uint_fast16_t)(h/2 + (cos(pc._phase*0.42f)*(h/2-PLASMA_CLOUD_MARGIN) ))
	};
	*/

	for( uint_fast16_t x=0; x<w; x++ ) {
		for( uint_fast16_t y=0; y<w; y++ ) {
			PointU16 d0 = (PointU16){ p0.x - x, p0.y - y };
			//Point16 d1 = (Point16){ p1.x - x, p1.y - y };

			uint_fast8_t distanceLookup = ((d0.x*d0.x)>>sqrtBitShift) + ((d0.y*d0.y)>>sqrtBitShift);
			uint_fast8_t aSqrt = sqrtTable[ distanceLookup ];

			uint_fast16_t color = tft.color565( aSqrt, ((aSqrt&0xfb)<<2), 0 );
			tft.drawPixel( x, y, color );
		}
	}

	//float audioPower = frameParams.audioMean;
	/*
	for( int_fast16_t x=0; x<w; x+=PLASMA_CLOUD_DITHER ) {
		for( int_fast16_t y=py._ditherY; y<h; y+=PLASMA_CLOUD_DITHER ) {
			Point16 d0 = (Point16){ p0.x - x, p0.y - y };
			Point16 d1 = (Point16){ p1.x - x, p1.y - y };

			//uint_fast16_t distance = sqrt( d0.x*d0.x + d0.y*d0.y );// * sqrt( d1.x*d1.x + d1.y*d1.y );	// SLOW
			//uint_fast16_t distance = abs( d0.x*d0.y );	// pretty good
			uint_fast16_t distance = abs( d0.x*d0.y + d1.x*d1.y );

			uint_fast8_t bright = lerp8( (uint_fast16_t)(distance >> 5) & 0xff, 0xff, audioPower );
			if( bright > 0x7f ) bright = 0xff - bright;
			bright <<= 2;

			uint_fast16_t color = tft.color565( bright, bright, 0 );

			tft.drawFastHLine( x, y, PLASMA_YELLOW_DITHER, color );
		}
	}
	*/
}

#endif
