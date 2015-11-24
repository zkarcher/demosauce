#ifndef PLASMA_CLOUD_H__
#define PLASMA_CLOUD_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"

const float PLASMA_CLOUD_SPEED = 0.02;
const uint_fast16_t PLASMA_CLOUD_MARGIN = 25;

const uint_fast8_t PLASMA_CLOUD_LINE_WIDTH = 8;
const uint_fast8_t PLASMA_CLOUD_STEP_Y = 4;

const float PLASMA_COLOR_CYCLE_SPEED = 0.003;

struct PlasmaCloudVars {
  float _phase;
	float _colorCycle;
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

	pc._phase += frameParams.timeMult * PLASMA_CLOUD_SPEED * (1.0f + frameParams.audioMean*5.0f);

	pc._colorCycle -= frameParams.timeMult * PLASMA_COLOR_CYCLE_SPEED * (1.0f + frameParams.audioMean*6.0f);
	if( pc._colorCycle <= 0.0f ) pc._colorCycle += 1.0f;
	uint_fast8_t colorAdd = pc._colorCycle * 0xff;

	//py._ditherY = (py._ditherY + 1) % PLASMA_YELLOW_DITHER;

	PointU16 p0 = (PointU16){
		(uint_fast16_t)(w/2 + (sin(pc._phase*0.32f)*(w/2-PLASMA_CLOUD_MARGIN) )),
		(uint_fast16_t)(h/2 + (sin(pc._phase*0.23f)*(h/2-PLASMA_CLOUD_MARGIN) ))
	};
	PointU16 p1 = (PointU16){
		(uint_fast16_t)(w/2 + (cos(pc._phase*1.07f)*(w/2-PLASMA_CLOUD_MARGIN) )),
		(uint_fast16_t)(h/2 + (cos(pc._phase*1.42f)*(h/2-PLASMA_CLOUD_MARGIN) ))
	};
	/*
	PointU16 p2 = (PointU16){
		(uint_fast16_t)(w/2 + (cos(pc._phase*0.57f)*(w/2-PLASMA_CLOUD_MARGIN) )),
		(uint_fast16_t)(h/2 + (cos(pc._phase*0.81f)*(h/2-PLASMA_CLOUD_MARGIN) ))
	};
	*/

	for( uint_fast16_t x=0; x<w; x+=PLASMA_CLOUD_LINE_WIDTH ) {
		for( uint_fast16_t y=pc._ditherY; y<h; y+=PLASMA_CLOUD_STEP_Y ) {
			PointU8 d0 = (PointU8){ abs(p0.x - x), abs(p0.y - y) };
			PointU8 d1 = (PointU8){ abs(p1.x - x), abs(p1.y - y) };
			//PointU8 d2 = (PointU8){ abs(p2.x - x), abs(p2.y - y) };

			uint_fast8_t lookup0 = (d0.x*d0.x + d0.y*d0.y) >> sqrtBitShift;
			uint_fast8_t lookup1 = (d1.x*d1.x + d1.y*d1.y) >> sqrtBitShift;
			//uint_fast8_t lookup2 = (d2.x*d2.x + d2.y*d2.y) >> sqrtBitShift;

			uint_fast8_t bright = (sqrtTable[ lookup0 ] * sqrtTable[ lookup1 ] ) >> 6;
			bright = (uint_fast16_t)(bright + colorAdd) & 0xff;

			uint_fast16_t color = tft.color565( ((bright&0xfb)<<2), 0, bright );
			tft.drawFastHLine( x, y, PLASMA_CLOUD_LINE_WIDTH, color );
		}
	}

	pc._ditherY = (pc._ditherY + 1) % PLASMA_CLOUD_STEP_Y;

}

#endif
