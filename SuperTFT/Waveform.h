#ifndef WAVEFORM_H__
#define WAVEFORM_H__

#include <Arduino.h>
#include <math.h>
#include "ILI9341_t3.h"

struct WaveformVars {
  uint_fast16_t _step;
  uint_fast8_t _colorPhase;
};
WaveformVars wv = (WaveformVars){0};

void waveform_setup( ILI9341_t3 tft ) {
  //uint_fast16_t w = tft.width();
  //uint_fast16_t h = tft.height();
  //tft.fillRect( 0, 0, w, h, 0x0 );
}

void waveform_perFrame( ILI9341_t3 tft, FrameParams frameParams ) {
  uint_fast16_t w = tft.width();
  uint_fast16_t h = tft.height();

  // fillRect: flickers pretty bad
  //tft.fillRect( 0, 0, w, h, LV_RED );

  // Prepare body color
  uint_fast8_t bright = (frameParams.audioPeak >> 1); // 0..512 -> 0..255
  uint_fast16_t bodyColor = tft.color565( bright, bright, 0 );  // yellow

  // Prepare body height
  uint_fast16_t h_2 = (h >> 1);
  uint_fast16_t bodyHeight = frameParams.audioMean * h;
  tft.drawFastVLine( wv._step, h_2-(bodyHeight>>1), bodyHeight, bodyColor );

  // Clear this column. Background should have a triangle wave effect
  uint_fast8_t bgBright = wv._colorPhase + wv._step;
  if( bgBright & 0x80 ) {
    bgBright = (0xff-bgBright) << 1;
  } else {
    bgBright <<= 1;
  }
  uint_fast16_t bgColor = tft.color565( 0, 0, bgBright );

  // Draw background color in the other pixels
  uint_fast16_t margin = (h - bodyHeight) >> 1;
  tft.drawFastVLine( wv._step, 0,          margin, bgColor );
  tft.drawFastVLine( wv._step, h - margin, margin, bgColor );

  // Advance to next line
  if( wv._step == 0 ) {
    wv._step = w - 1; // Restart drawing on right-hand size

    // After the screen is covered: Advance the background color
    wv._colorPhase += 3;

  } else {
    wv._step--;
  }

}

#endif
