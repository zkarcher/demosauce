#ifndef SQUARES_H__
#define SQUARES_H__

#include <Arduino.h>
#include "ILI9341_t3.h"

struct SquareVars {
  uint_fast8_t _step;
};
SquareVars sq = (SquareVars){0};

const uint_fast8_t SQ_SIZE = 16;

void squares_perFrame( ILI9341_t3 tft ) {
  uint_fast16_t w = tft.width();
  uint_fast16_t h = tft.height();
  
  for( uint_fast8_t i=0; i<w; i+=SQ_SIZE ) {
    for( uint_fast8_t j=0; j<h; j+=SQ_SIZE ) {

      tft.drawRect( i, j, SQ_SIZE, SQ_SIZE, 0x0 );
      //tft.fillRect( i, j, SQ_SIZE, SQ_SIZE, 0x0 );
            
      uint_fast8_t bright = (i+j+sq._step) & 0xff;
      uint_fast16_t clr = tft.color565( bright, 0, bright );

      uint_fast8_t rando = random(4);
      tft.drawRect( i+rando, j+rando, SQ_SIZE-2*rando, SQ_SIZE-2*rando, clr );
    }
  }

  //delay(10);
  
  sq._step += 0x1f;
}

#endif
