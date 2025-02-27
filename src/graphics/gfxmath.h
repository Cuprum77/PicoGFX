#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <math.h>
#include <stdint.h>

// Preprocessor definitions
#define NUMBER_OF_ANGLES 3600	// decides how many angles to precompute, additional zeroes for more precision
#define ANGLE_SCALE NUMBER_OF_ANGLES / 360	// used to make the angle input more intuitive
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062
#define FIXED_POINT_SCALE 4096	// 2^12
#define FIXED_POINT_SCALE_BITS 12	// log2(FIXED_POINT_SCALE)
#define FIXED_POINT_SCALE_HIGH_RES 16777216  // 2^24
#define FIXED_POINT_SCALE_HIGH_RES_BITS 24	// log2(FIXED_POINT_SCALE_HIGH_RES)

extern int32_t cosTable[NUMBER_OF_ANGLES];
extern int32_t sinTable[NUMBER_OF_ANGLES];
extern int32_t tanTable[NUMBER_OF_ANGLES];
extern int32_t atanTable[NUMBER_OF_ANGLES];

extern int32_t imin(int32_t x, int32_t y);
extern int32_t imax(int32_t x, int32_t y);
extern int32_t iabs(int32_t x);
extern int32_t isqrt(int32_t x);
extern int32_t ipow(int32_t x, int32_t y);
extern int32_t ifactorial(int32_t x);

extern void fillLookupTables();
extern void pointOnCircle(int32_t radius, int32_t angle, int32_t offsetX, int32_t offsetY, int32_t* x, int32_t* y);

extern int32_t lerp(int32_t v0, int32_t v1, int32_t t);
extern int32_t clamp(int32_t x, int32_t min, int32_t max);

#ifdef __cplusplus
}
#endif