#ifndef SPHERE_3D_H__
#define SPHERE_3D_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"

const float SPHERE_3D_ROTATE_SPEED = 0.005f;
const float SPHERE_3D_TILT_SPEED = 2.0f;
const float SPHERE_3D_TILT_AMOUNT = 0.4f;

// 3D render
const float SPHERE_DISTANCE = 2.5f;
const float SPHERE_OUTER_MULT = 1.414f;  // spike size

struct Sphere3DVars {
  float _rotatePhase;
  uint_fast16_t _baseCircSize;
  uint_fast16_t _circStep;
  uint_fast8_t _sparkle;

  uint_fast16_t _bgColor;
};
Sphere3DVars s3d = (Sphere3DVars){ 0 };

void sphere3D_setup( ILI9341_t3 tft ) {
  uint_fast16_t w = tft.width();
  uint_fast16_t h = tft.height();

  s3d._bgColor = tft.color565( 0, 0x44, 0x44 );

  Point16 sphereTopOnScreen = xyz2screen( 0, -1.0f, SPHERE_DISTANCE, (w>>1), (h>>1) );
  s3d._baseCircSize = (h/2) - sphereTopOnScreen.y + 1;
}

uint_fast16_t sphere3D_bgColor(){
	return s3d._bgColor;
}

void sphere3D_drawLine( ILI9341_t3 tft, float cosTilt, float sinTilt, float x, float y, float z, uint_fast16_t w_2, uint_fast16_t h_2, uint_fast16_t color ) {
  // Tilt!
  float tempY = y;
  y = tempY*cosTilt + z*sinTilt;
  z = z*cosTilt - tempY*sinTilt;

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

	s3d._sparkle = max( (frameParams.audioPeak >> 1), s3d._sparkle * (1.0f-(frameParams.timeMult*0.02f)) );
	uint_fast16_t erase = s3d._bgColor;

	float x, y, z, sinLat;

  float oldTilt = -sin( oldPhase * SPHERE_3D_TILT_SPEED ) * SPHERE_3D_TILT_AMOUNT;
  float oldCosTilt = cos( oldTilt );
  float oldSinTilt = sin( oldTilt );

  float tilt = -sin( s3d._rotatePhase * SPHERE_3D_TILT_SPEED ) * SPHERE_3D_TILT_AMOUNT;
  float cosTilt = cos( tilt );
  float sinTilt = sin( tilt );

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
			sphere3D_drawLine( tft, oldCosTilt, oldSinTilt, x, y, z, w_2, h_2, erase );
			sphere3D_drawLine( tft, oldCosTilt, oldSinTilt, -x, y, -z, w_2, h_2, erase );
			sphere3D_drawLine( tft, oldCosTilt, oldSinTilt, -z, y, x, w_2, h_2, erase );
			sphere3D_drawLine( tft, oldCosTilt, oldSinTilt, z, y, -x, w_2, h_2, erase );

			// Draw the other hemisphere
			sphere3D_drawLine( tft, oldCosTilt, oldSinTilt, x, -y, z, w_2, h_2, erase );
			sphere3D_drawLine( tft, oldCosTilt, oldSinTilt, -x, -y, -z, w_2, h_2, erase );
			sphere3D_drawLine( tft, oldCosTilt, oldSinTilt, -z, -y, x, w_2, h_2, erase );
			sphere3D_drawLine( tft, oldCosTilt, oldSinTilt, z, -y, -x, w_2, h_2, erase );

			// Now, draw the new lines

      uint_fast16_t color = tft.color565(
        random( s3d._sparkle>>1, s3d._sparkle ),
        random( s3d._sparkle>>1, s3d._sparkle ),
        0xff
      );

			x = cos( s3d._rotatePhase + lon );
			z = sin( s3d._rotatePhase + lon );

			// Now we need the y (up & down), then normalize x & z to create a normalized 3D vector (length == 1.0)
			y = cos( lat );
			sinLat = sin( lat );
			x *= sinLat;
			z *= sinLat;

			// We can swap & negate x,y,z to draw at least 8 lines without recomputing cos & sin values etc
			sphere3D_drawLine( tft, cosTilt, sinTilt, x, y, z, w_2, h_2, color );
			sphere3D_drawLine( tft, cosTilt, sinTilt, -x, y, -z, w_2, h_2, color );
			sphere3D_drawLine( tft, cosTilt, sinTilt, -z, y, x, w_2, h_2, color );
			sphere3D_drawLine( tft, cosTilt, sinTilt, z, y, -x, w_2, h_2, color );

			// Draw the other hemisphere
			sphere3D_drawLine( tft, cosTilt, sinTilt, x, -y, z, w_2, h_2, color );
			sphere3D_drawLine( tft, cosTilt, sinTilt, -x, -y, -z, w_2, h_2, color );
			sphere3D_drawLine( tft, cosTilt, sinTilt, -z, -y, x, w_2, h_2, color );
			sphere3D_drawLine( tft, cosTilt, sinTilt, z, -y, -x, w_2, h_2, color );
		}
	}

  s3d._circStep++;
  if( s3d._circStep > 10 ) s3d._circStep = 0;

  tft.drawCircle( w_2, h_2, s3d._baseCircSize + s3d._circStep, tft.color565( 0, random((10-s3d._circStep)*5), random((10-s3d._circStep)*10) ) | s3d._bgColor );

}

#endif
