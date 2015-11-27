#include "SPI.h"
#include "ILI9341_t3.h"

#define MIC_PIN (14)
#define BACKLIGHT_PIN (23)

#include "FrameParams.h"

// Animations
/*
#include "Checkerboard.h"
#include "Cube3D.h"
#include "Leaves.h"
#include "MagentaSquares.h"
#include "MicCheck.h"
#include "PlasmaCloud.h"
#include "PlasmaYellow.h"
#include "Sphere3D.h"
#include "TriangleWeb.h"
*/
#include "TwistyText.h"
//#include "Waveform.h"

// Transitions
#include "TransitionSquares.h"

const boolean DEBUG_MODE = true; // dev: for hacking on one animation.
const uint_fast8_t DEBUG_ANIM_INDEX = 0;

const int_fast16_t DEFAULT_ANIM_TIME = 20 * 1000;  // ms

const uint8_t TFT_DC = 9;
const uint8_t TFT_CS = 10;

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
FrameParams frameParams;
long previousMillis = 0;

/*
Checkerboard _checkerboard;
Cube3D _cube3D;
Leaves _leaves;
MagentaSquares _magentaSquares;
PlasmaCloud _plasmaCloud;
PlasmaYellow _plasmaYellow;
Sphere3D _sphere3D;
TriangleWeb _triangleWeb;
*/
TwistyText *_twistyText;
//Waveform _waveform;

BaseAnimation **anims; // Array of pointers to BaseAnimation's. Initialized in setup() below.
int_fast8_t animCount;

BaseAnimation *activeAnim = 0;
int_fast16_t animTimeLeft = DEFAULT_ANIM_TIME;

BaseAnimation *nextAnim;

boolean isTransition = true;

// Search the anims[] aray for the activeAnim pointer. If found, return the array index.
int_fast8_t getActiveAnimIndex() {
  for( int_fast8_t i=0; i<animCount; i++ ) {
    if( anims[i] == activeAnim ) return i;
  }
  return -1;  // not found
}

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

  /*
  _checkerboard = Checkerboard( tft );
  _cube3D = Cube3D( tft );
  _leaves = Leaves( tft );
  _magentaSquares = MagentaSquares( tft );
  _plasmaCloud = PlasmaCloud( tft );
  _plasmaYellow = PlasmaYellow( tft );
  _sphere3D = Sphere3D( tft );
  _triangleWeb = TriangleWeb( tft );
  */
  _twistyText = new TwistyText();
  //_waveform = Waveform( tft );

  /*
  anims[] = {
    &_twistyText
    &_plasmaCloud,
    &_waveform,
    &_magentaSquares,
    &_sphere3D,
    &_checkerboard,
    &_leaves,
    &_cube3D,
    &_plasmaYellow,
    &_triangleWeb
  };
  */

  // Populate anims in the order you want them to display.
  BaseAnimation* ANIMS_TEMP[] = {
    _twistyText
  };
  animCount = sizeof( ANIMS_TEMP ) / sizeof( BaseAnimation* );

  // Store ANIMS_TEMP as permanent anims
  anims = (BaseAnimation**)malloc( animCount * sizeof(BaseAnimation*) );
  for( int_fast8_t i=0; i<animCount; i++ ) {
    anims[i] = ANIMS_TEMP[i];
  }

  // Initialize all animations
  for( int_fast8_t i=0; i<animCount; i++ ) {
    anims[i]->init( tft );
  }

  if( !activeAnim ) {
    if( DEBUG_MODE ) {
      startAnimation( anims[DEBUG_ANIM_INDEX] );
    } else {
      startAnimation( anims[0] );
    }
  }
}

void startAnimation( BaseAnimation *newAnim ) {
  isTransition = false;

  activeAnim = newAnim;
  tft.fillScreen( activeAnim->bgColor() );
  activeAnim->reset( tft );

  animTimeLeft = DEFAULT_ANIM_TIME;
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

  frameParams.audioPeak = min( (uint_fast16_t)frameParams.audioPeak, (uint_fast16_t)511 );

  if( !isTransition ) {
    //((BaseAnimation*)_twistyText)->perFrame( tft, frameParams );
    activeAnim->perFrame( tft, frameParams );
    animTimeLeft -= elapsed;
  }

  // Has this animation expired?
  boolean canTransition = activeAnim->canTransition();

  if( !DEBUG_MODE && canTransition && (animTimeLeft <= 0) ) {

    // If the transition has not started yet, then start it.
    if( !isTransition ) {
      isTransition = true;
      nextAnim = anims[ (getActiveAnimIndex() + 1) % animCount ];

      transitionSquares_reset( tft, nextAnim->bgColor() );
    }

    // After the transition ends, advance to the next animation
    boolean isTransitionComplete = transitionSquares_perFrame( tft, frameParams );
    if( isTransitionComplete ) {
      startAnimation( nextAnim );
    }

  }

}
