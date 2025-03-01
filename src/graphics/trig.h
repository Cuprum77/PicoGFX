#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <pico/divider.h>

// Preprocessor definitions
#define ANGLE_STEP 10 // Resolution of the lookup tables, in steps per degree
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062 // self explanatory

#define FIXED_POINT_SCALE 65536	// 2^16
#define FIXED_POINT_SCALE_BITS 16	// log2(FIXED_POINT_SCALE)
#define FIXED_POINT_SCALE_HIGH_RES 16777216  // 2^24
#define FIXED_POINT_SCALE_HIGH_RES_BITS 24	// log2(FIXED_POINT_SCALE_HIGH_RES)

#define COS_SIZE 900
#define COS_MULTIPLIER FIXED_POINT_SCALE
#define COS_MULTIPLIER_BITS FIXED_POINT_SCALE_BITS

#define SIN_SIZE 900
#define SIN_MULTIPLIER FIXED_POINT_SCALE
#define SIN_MULTIPLIER_BITS FIXED_POINT_SCALE_BITS

#define TAN_SIZE 3600
#define TAN_MULTIPLIER FIXED_POINT_SCALE
#define TAN_MULTIPLIER_BITS FIXED_POINT_SCALE_BITS

#define ATAN_SIZE 3600
#define ATAN_MULTIPLIER FIXED_POINT_SCALE
#define ATAN_MULTIPLIER_BITS FIXED_POINT_SCALE_BITS

extern int32_t sinTable[SIN_SIZE];
extern int32_t tanTable[TAN_SIZE];
extern int32_t atanTable[ATAN_SIZE];

extern int32_t isin(int32_t angle);
extern int32_t isind(int32_t angle);
extern int32_t icos(int32_t angle);
extern int32_t icosd(int32_t angle);
extern int32_t itan(int32_t angle);
extern int32_t itand(int32_t angle);
extern int32_t iatan(int32_t angle);
extern int32_t iatand(int32_t angle);

extern int32_t deg(int32_t rad);
extern int32_t rad(int32_t deg);

extern int32_t inorm(int32_t angle);
extern int32_t inormlim(int32_t angle, int32_t min, int32_t max, int32_t step);

#ifdef __cplusplus
}
#endif