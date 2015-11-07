#ifndef FFT_CHECK_H__
#define FFT_CHECK_H__

#include <Arduino.h>
#include "ILI9341_t3.h"


/*
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioInputAnalog         adc1( MIC_PIN );           //xy=218,102
AudioAnalyzeFFT256       fft256_1;       //xy=411,109
AudioConnection          patchCord1( adc1, fft256_1 );
// GUItool: end automatically generated code
*/



void fftCheck_setup(){
  
}

void fftCheck_perFrame( ILI9341_t3 tft ) {

  /*

  uint_fast16_t w = tft.width();
  uint_fast16_t h = tft.height();

  // Clear
  tft.fillRect( 0, 0, w, h, 0x0 );
*/

/*
  const uint_fast8_t FFT_N = 256;
  for ( uint_fast8_t i = 0 ; i < FFT_N ; i++ ) { // collect samples
    fht_input[i] =  analogRead(MIC_PIN) - 512;  // unsigned int to signed int
  }
  fht_window(); // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run(); // process the data in the fht
  */
}

#endif
