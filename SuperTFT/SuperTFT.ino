#include "SPI.h"
#include "ILI9341_t3.h"

#define MIC_PIN (14)
#define BACKLIGHT_PIN (23)

#include "FrameParams.h"
#include "MagentaSquares.h"
#include "MicCheck.h"
#include "Leaves.h"
#include "Waveform.h"
#include "TriangleWeb.h"
#include "OrangeGrams.h"
#include "GreenCycle.h"

const uint8_t TFT_DC = 9;
const uint8_t TFT_CS = 10;

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
FrameParams frameParams;
long previousMillis = 0;

void setup() {
  // Backlight
  pinMode( BACKLIGHT_PIN, OUTPUT );
  analogWrite( BACKLIGHT_PIN, 1024 );

  // Microphone
  pinMode( MIC_PIN, INPUT );

  tft.begin();

  // Clear
  uint16_t w = tft.width();
  uint16_t h = tft.height();
  tft.fillRect( 0, 0, w, h, 0x0 );

  tft.setRotation( 3 ); // ribbon cable on left
  tft.setScroll( 0 );
  tft.setColorSetDefault();

  // After rotation is set: Prepare animations
  //leaves_start( tft );
  waveform_setup( tft );
  triangleWeb_setup( tft );
  magentaSquares_setup( tft );
  greenCycle_setup( tft );
}

void loop() {
  // Frame multiplier
  long newMillis = millis();
  uint_fast8_t elapsed = newMillis - previousMillis;
  previousMillis = newMillis;
  frameParams.timeMult = elapsed * (60.0f / 1000);  // 1.0==exactly 60fps. 4.0==15fps, 4x slower

  // Get some audio input
  const uint_fast8_t SAMPLES_PER_FRAME = 1;
  frameParams.audioPeak = 0;
  uint_fast16_t sum = 0;

  for( uint_fast8_t s=0; s<SAMPLES_PER_FRAME; s++ ) {
    uint_fast16_t sample = abs( analogRead( MIC_PIN ) - 511 );
    frameParams.audioPeak = max( frameParams.audioPeak, sample );
    sum += sample;
  }
  frameParams.audioMean = sum * (1.0 / (512*SAMPLES_PER_FRAME));  // Range: 0..1

  // TESTING:
  //micCheck_perFrame( tft, frameParams );

  // TODO: Add mic reactivity:
  greenCycle_perFrame( tft, frameParams );

  // TODO: Animations need work:
  //orangeGrams_perFrame( tft, frameParams );

  // Working:
  //triangleWeb_perFrame( tft, frameParams ); // greyscale with slight magenta tint (because 565 color?)
  //waveform_perFrame( tft, frameParams );  // blue background, yellow waveform
  //leaves_perFrame( tft, frameParams );  // magenta background, red/white sweeping leaves
  //magentaSquares_perFrame( tft, frameParams );

}
