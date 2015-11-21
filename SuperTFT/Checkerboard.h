#ifndef CHECKERBOARD_H__
#define CHECKERBOARD_H__

#include <Arduino.h>
#include "ILI9341_t3.h"
#include "MathUtil.h"

const uint_fast8_t CHECKERBOARD_SIZE_MIN = 12;
const uint_fast8_t CHECKERBOARD_SIZE_MAX = 60;

const float CHECKERBOARD_ZOOM_SPEED = 0.03;

struct CheckerboardVars {
  float _phase;
};
CheckerboardVars cb = (CheckerboardVars){0};

void checkerboard_setup( ILI9341_t3 tft ) {
  //uint_fast16_t w = tft.width();
  //uint_fast16_t h = tft.height();
}

void checkerboard_perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  int_fast16_t w = (int_fast16_t)tft.width();
  int_fast16_t h = (int_fast16_t)tft.height();

  cb._phase += CHECKERBOARD_ZOOM_SPEED * frameParams.timeMult;

  float sizer = (cos(cb._phase) + 1.0) * 0.5; // Range 0..1
  uint_fast8_t size = lerp( CHECKERBOARD_SIZE_MIN, CHECKERBOARD_SIZE_MAX, sizer );

  uint_fast16_t colorDark = tft.color565( 0xff, 0, 0 );
  uint_fast16_t colorLight = tft.color565( 0xff, 0xff, 0xff );

  // Zoom in & out from center
  uint_fast8_t sqLeftOfCenter = ceil( (w/2) / (float)size );
  int_fast16_t startX = (w/2) - sqLeftOfCenter * size;
  uint_fast8_t sqAboveCenter = ceil( (h/2) / (float)size );
  int_fast16_t startY = (h/2) - sqAboveCenter * size;

  // First square is light or 8dark?
  boolean startLight = (sqLeftOfCenter%2) ^ (sqAboveCenter%2);

  for( int_fast16_t x=startX; x<w; x+=size ) {
    boolean isLight = startLight;

    int16_t drawX = max( x, 0 );
    int16_t drawW = (x >= 0) ? size : (size+x);

    for( int_fast16_t y=startY; y<h; y+=size ) {
      int16_t drawY = max( y, 0 );
      int16_t drawH = (y >= 0) ? size : (size+y);

      tft.fillRect( drawX, drawY, drawW, drawH, (isLight ? colorLight : colorDark) );
      isLight = !isLight;
    }

    startLight = !startLight;
  }

}

#endif
