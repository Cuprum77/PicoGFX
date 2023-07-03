#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <math.h>

// Preprocessor definitions
#define NUMBER_OF_ANGLES 3600	// decides how many angles to precompute, additional zeroes for more precision
#define ANGLE_SCALE NUMBER_OF_ANGLES / 360	// used to make the angle input more intuitive
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062
#define FIXED_POINT_SCALE 4096	// 2^12
#define NUMBER_OF_TERMS 4096	// used for the Taylor series

extern int cosTable[NUMBER_OF_ANGLES];
extern int sinTable[NUMBER_OF_ANGLES];
extern int tanTable[NUMBER_OF_ANGLES];

extern int imin(int x, int y);
extern int imax(int x, int y);
extern int iabs(int x);
extern unsigned int isqrt(unsigned int x);
extern unsigned int ipow(unsigned int x, unsigned int y);
extern unsigned int ifactorial(unsigned int x);

extern void fillLookupTables();

#ifdef __cplusplus
}
#endif