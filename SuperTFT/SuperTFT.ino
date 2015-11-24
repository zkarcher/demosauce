#include "SPI.h"
#include "ILI9341_t3.h"

#define MIC_PIN (14)
#define BACKLIGHT_PIN (23)

#include "FrameParams.h"

// Animations
#include "MagentaSquares.h"
#include "MicCheck.h"
#include "Leaves.h"
#include "Waveform.h"
#include "TriangleWeb.h"
#include "OrangeGrams.h"
#include "Checkerboard.h"
#include "Cube3D.h"
#include "Sphere3D.h"
#include "TwistyText.h"

// Transitions
#include "TransitionSquares.h"

const boolean DO_TRANSITIONS = false; // dev: when hacking on one animation, set this to false
const int_fast16_t DEFAULT_ANIM_TIME = 10 * 1000;  // ms

const uint8_t TFT_DC = 9;
const uint8_t TFT_CS = 10;

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
FrameParams frameParams;
long previousMillis = 0;

enum animationType {
  kAnimTriangleWeb = 0,
  kAnimTwistyText,
  kAnimCheckerboard,
  kAnimMagentaSquares,
  kAnimLeaves,
  kAnimWaveform,
  kAnimCube3D,
  kAnimSphere3D,
  kAnim_COUNT
};

int_fast8_t activeAnim = -1;
int_fast16_t animTimeLeft = DEFAULT_ANIM_TIME;
int_fast8_t nextAnim = -1;

boolean isTransition = true;

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
  leaves_setup( tft );
  twistyText_setup( tft );
  checkerboard_setup( tft );
  waveform_setup( tft );
  triangleWeb_setup( tft );
  magentaSquares_setup( tft );
  cube3D_setup( tft );
  sphere3D_setup( tft );

  if( activeAnim == -1 ) {
    startAnimation( 0 );
  }
}

int_fast16_t animationBGColor( int_fast8_t anim ){
  switch( anim ) {
    case kAnimTwistyText:       return twistyText_bgColor();
    case kAnimCheckerboard:     return checkerboard_bgColor(); break;
    case kAnimMagentaSquares:   return magentaSquares_bgColor(); break;
    case kAnimLeaves:           return leaves_bgColor(); break;
    case kAnimWaveform:         return waveform_bgColor(); break;
    case kAnimTriangleWeb:      return triangleWeb_bgColor(); break;
    case kAnimCube3D:           return cube3D_bgColor(); break;
    case kAnimSphere3D:         return sphere3D_bgColor(); break;
    default:                    return tft.color565( random(0xff), random(0xff), random(0xff) );
  }
  return 0x0;
}

void startAnimation( int_fast8_t anim ) {
  isTransition = false;

  activeAnim = anim;
  animTimeLeft = DEFAULT_ANIM_TIME;
  tft.fillScreen( animationBGColor( activeAnim ) );

  switch( activeAnim ) {
    case kAnimTwistyText:       twistyText_reset( tft ); break;
    default:                    break;
  }
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

  // TODO: Animations need work:
  //orangeGrams_perFrame( tft, frameParams );

  // TODO: Add mic reactivity:
  //checkerboard_perFrame( tft, frameParams );
  //twistyText_perFrame( tft, frameParams );

  //sphere3D_perFrame( tft, frameParams );

  // Working:
  //triangleWeb_perFrame( tft, frameParams ); // greyscale with slight magenta tint (because 565 color?)
  //waveform_perFrame( tft, frameParams );  // blue background, yellow waveform
  //leaves_perFrame( tft, frameParams );  // magenta background, red/white sweeping leaves
  //magentaSquares_perFrame( tft, frameParams );
  //cube3D_perFrame( tft, frameParams );

  if( !isTransition ) {

    switch( activeAnim ) {
      case kAnimTwistyText:        twistyText_perFrame( tft, frameParams ); break;
      case kAnimCheckerboard:      checkerboard_perFrame( tft, frameParams ); break;
      case kAnimMagentaSquares:    magentaSquares_perFrame( tft, frameParams ); break;
      case kAnimLeaves:            leaves_perFrame( tft, frameParams ); break;
      case kAnimWaveform:          waveform_perFrame( tft, frameParams ); break;
      case kAnimTriangleWeb:       triangleWeb_perFrame( tft, frameParams ); break;
      case kAnimCube3D:            cube3D_perFrame( tft, frameParams ); break;
      case kAnimSphere3D:          sphere3D_perFrame( tft, frameParams ); break;
      case kAnim_COUNT:            break;
    }

    animTimeLeft -= elapsed;

  }

  // Has this animation expired?
  boolean canTransition = true;
  if( activeAnim == kAnimTwistyText ) {
    canTransition = twistyText_isComplete();
  }

  if( DO_TRANSITIONS && canTransition && (animTimeLeft <= 0) ) {

    // If the transition has not started yet, then start it.
    if( !isTransition ) {
      isTransition = true;
      nextAnim = (activeAnim + 1) % (int_fast8_t)(kAnim_COUNT);

      transitionSquares_reset( tft, animationBGColor(nextAnim) );
    }

    // After the transition ends, advance to the next animation
    boolean isTransitionComplete = transitionSquares_perFrame( tft, frameParams );
    if( isTransitionComplete ) {
      startAnimation( nextAnim );
    }

  }

}
