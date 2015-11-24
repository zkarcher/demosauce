#ifndef SPHERE_3D_H__
#define SPHERE_3D_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"

const float SPHERE_3D_ROTATE_SPEED = 0.005f;
const float SPHERE_3D_COLOR_SPEED = 0.013f;

struct Sphere3DVars {
  float _rotatePhase;
	float _colorPhase;

  uint_fast16_t _bgColor;
};
Sphere3DVars s3d = (Sphere3DVars){ 0 };

void sphere3D_setup( ILI9341_t3 tft ) {
  //uint_fast16_t w = tft.width();
  //uint_fast16_t h = tft.height();

  s3d._bgColor = tft.color565( 0, 0x66, 0x66 );
}

uint_fast16_t sphere3D_bgColor(){
	return s3d._bgColor;
}

void sphere3D_drawLine( ILI9341_t3 tft, float x, float y, float z, uint_fast16_t w_2, uint_fast16_t h_2, uint_fast16_t color ) {
	const float SPHERE_DISTANCE = 2.5;
	const float SPHERE_OUTER_MULT = 1.4;

	Point16 innerPt = xyz2screen( x, y, z+SPHERE_DISTANCE, w_2, h_2 );
	Point16 outerPt = xyz2screen( x*SPHERE_OUTER_MULT, y*SPHERE_OUTER_MULT, z*SPHERE_OUTER_MULT + SPHERE_DISTANCE, w_2, h_2 );
	tft.drawLine( innerPt.x, innerPt.y, outerPt.x, outerPt.y, color );

	// Just for kicks... Let's draw some circumference lines
	const float T_BAR_RADIUS = 0.1f;
	Point16 p0 = xyz2screen( x-z*T_BAR_RADIUS, y, z+x*T_BAR_RADIUS + SPHERE_DISTANCE, w_2, h_2 );
	Point16 p1 = xyz2screen( x+z*T_BAR_RADIUS, y, z-x*T_BAR_RADIUS + SPHERE_DISTANCE, w_2, h_2 );
	tft.drawLine( p0.x, p0.y, p1.x, p1.y, color );

}

void sphere3D_perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  uint_fast16_t w = (uint_fast16_t)tft.width();
  uint_fast16_t h = (uint_fast16_t)tft.height();

	uint_fast16_t w_2 = (w>>1);
	uint_fast16_t h_2 = (h>>1);

	float oldPhase = s3d._rotatePhase;
  s3d._rotatePhase += frameParams.timeMult * SPHERE_3D_ROTATE_SPEED;
	s3d._colorPhase += frameParams.timeMult * SPHERE_3D_COLOR_SPEED;

	uint_fast8_t bright = (uint_fast8_t)( 0xff * (s3d._colorPhase - floor( s3d._colorPhase )) );
	uint_fast16_t color = tft.color565( bright, bright, 0xff );
	uint_fast16_t erase = s3d._bgColor;

	float x, y, z, sinLat;

	// Rotating sphere yo
	for( float lon=0.0f; lon<(M_PI*0.5f); lon+=(M_PI*0.125f) ) {	// longitude (around). Only 1/4 of sphere circumference.
		for( float lat=(M_PI*0.0625f); lat<(M_PI*0.5f); lat+=(M_PI*0.125f) ) {	// latitude (up & down). Only 1/2 of sphere height.

			// Erase the old line here
			x = cos( oldPhase + lon );
			z = sin( oldPhase + lon );

			y = cos( lat );
			sinLat = sin( lat );
			x *= sinLat;
			z *= sinLat;

			// We can swap & negate x,y,z to draw at least 8 lines without recomputing cos & sin values etc
			sphere3D_drawLine( tft, x, y, z, w_2, h_2, erase );
			sphere3D_drawLine( tft, -x, y, -z, w_2, h_2, erase );
			sphere3D_drawLine( tft, -z, y, x, w_2, h_2, erase );
			sphere3D_drawLine( tft, z, y, -x, w_2, h_2, erase );

			// Draw the other hemisphere
			sphere3D_drawLine( tft, x, -y, z, w_2, h_2, erase );
			sphere3D_drawLine( tft, -x, -y, -z, w_2, h_2, erase );
			sphere3D_drawLine( tft, -z, -y, x, w_2, h_2, erase );
			sphere3D_drawLine( tft, z, -y, -x, w_2, h_2, erase );

			// Now, draw the new line
			x = cos( s3d._rotatePhase + lon );
			z = sin( s3d._rotatePhase + lon );

			// Now we need the y (up & down), then normalize x & z to create a normalized 3D vector (length == 1.0)
			y = cos( lat );
			sinLat = sin( lat );
			x *= sinLat;
			z *= sinLat;

			// We can swap & negate x,y,z to draw at least 8 lines without recomputing cos & sin values etc
			sphere3D_drawLine( tft, x, y, z, w_2, h_2, color );
			sphere3D_drawLine( tft, -x, y, -z, w_2, h_2, color );
			sphere3D_drawLine( tft, -z, y, x, w_2, h_2, color );
			sphere3D_drawLine( tft, z, y, -x, w_2, h_2, color );

			// Draw the other hemisphere
			sphere3D_drawLine( tft, x, -y, z, w_2, h_2, color );
			sphere3D_drawLine( tft, -x, -y, -z, w_2, h_2, color );
			sphere3D_drawLine( tft, -z, -y, x, w_2, h_2, color );
			sphere3D_drawLine( tft, z, -y, -x, w_2, h_2, color );
		}
	}

}

#endif
