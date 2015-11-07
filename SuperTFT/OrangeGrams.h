#ifndef ORANGE_GRAMS_H__
#define ORANGE_GRAMS_H__

#include <Arduino.h>
#include <math.h>
#include "ILI9341_t3.h"

struct OrangeGramsVars {
  uint_fast16_t _step;
};
OrangeGramsVars og = (OrangeGramsVars){0};

void orangeGrams_start( ILI9341_t3 tft ) {
  uint_fast16_t w = tft.width();
  uint_fast16_t h = tft.height();
  tft.fillRect( 0, 0, w, h, 0x0 );
}

void orangeGrams_perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  uint_fast16_t w = tft.width();
  uint_fast16_t h = tft.height();
  
  // fillRect: flickers pretty bad
  //tft.fillRect( 0, 0, w, h, LV_RED );

  uint_fast16_t idx = (og._step >> 6);
  uint_fast16_t offset = 63 - (og._step - (idx << 6));

  for( uint_fast8_t i=0; i<4; i++ ) {
    for( uint_fast8_t j=0; j<6; j++ ) {
      uint_fast8_t hash = idx + (i*4) + j;
      uint_fast8_t r = hash * 37;
      uint_fast8_t g = ((hash * 54) * r) >> 8;
      uint_fast16_t color = tft.color565( r, g, 0 );

      tft.drawFastHLine( i*64, j*64+offset, 63, color );
    }
  }

  //delay( 10 );
  
  /* 
  triWeb._phase += frameParams.timeMult * 0.0005 * (frameParams.audioMean*3.0f + 1.0f);
  
  // Prepare web color
  uint_fast8_t bright = (frameParams.audioPeak >> 1); // 0..512 -> 0..255
  uint_fast16_t color = tft.color565( bright, bright, bright );

  // TODO: It is possible to re-use some Point's here
  Point nw, ne, sw;
  for( uint_fast8_t i=0; i<4; i++ ) { // step across
    nw = getWebPoint( i, 0, triWeb._phase );

    for( uint_fast8_t j=0; j<6; j++ ) { // step down
      ne = getWebPoint( i+1, j, triWeb._phase );
      sw = getWebPoint( i, j+1, triWeb._phase );

      tft.drawLine( nw.x, nw.y, ne.x, ne.y, color );
      tft.drawLine( nw.x, nw.y, sw.x, sw.y, color );
      tft.drawLine( ne.x, ne.y, sw.x, sw.y, color );

      // Re-use sw point as new nw
      nw = sw;
    }
  }
  */

  og._step++;
}

#endif
