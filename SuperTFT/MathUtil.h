#ifndef MATH_UTIL_H__
#define MATH_UTIL_H__

uint_fast8_t lerp( uint_fast8_t a, uint_fast8_t b, float progress ){
	return a + (b-a)*progress;
}

float lerp( float a, float b, float progress ){
	return a + (b-a)*progress;
}

#endif
