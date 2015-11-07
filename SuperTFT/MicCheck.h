#ifndef MIC_CHECK_H__
#define MIC_CHECK_H__

#include <Arduino.h>
#include "ILI9341_t3.h"

struct MicCheckVars {
  int_fast16_t lastAmp;
};
MicCheckVars mc = (MicCheckVars){0};

void micCheck_perFrame( ILI9341_t3 tft ) {
  const uint_fast8_t SAMPLES_PER_FRAME = 8;

  uint_fast16_t w = tft.width();
  uint_fast16_t h = tft.height();

  int_fast16_t amp = 0;
  for( uint_fast8_t s=0; s<SAMPLES_PER_FRAME; s++ ) {
    // analogRead: range [0..1024] . No energy == 512.
    amp += abs( analogRead( MIC_PIN ) - 512 );
  }
  
  float amp_f = amp * (1.0/(SAMPLES_PER_FRAME*512));  // range [0..1]

  // Weight so it appears louder
  amp_f = (1.0-amp_f);
  amp_f *= amp_f;
  amp_f *= amp_f;
  amp_f = (1.0-amp_f);
  
  amp = amp_f * (max(w,h)*0.5); // Scale so full amp == max width/height of screen

  // Hollow out the previous circle
  if( (amp < mc.lastAmp) && (mc.lastAmp > 1) ) {
    tft.fillCircle( w/2, h/2, mc.lastAmp-1, 0x0 );
  }

  uint_fast8_t bright = ((1.0f-amp_f) * 0x7f) + 0x80;
  tft.fillCircle( w/2, h/2, amp, tft.color565( bright, bright, bright ) );

  mc.lastAmp = amp;
}

#endif
