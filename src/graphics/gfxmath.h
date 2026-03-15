#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "trig.h"
#include <math.h>
#include <stdint.h>

extern int32_t imin(int32_t x, int32_t y);
extern int32_t imax(int32_t x, int32_t y);
extern int32_t iabs(int32_t x);
extern int32_t isqrt(int32_t x);
extern int32_t ipow(int32_t x, int32_t y);
extern int32_t ifactorial(int32_t x);

extern void pcircle(int32_t radius, int32_t angle, int32_t offsetX, int32_t offsetY, int32_t* x, int32_t* y);

extern int32_t lerp(int32_t v0, int32_t v1, int32_t t);
extern int32_t clamp(int32_t x, int32_t min, int32_t max);

#ifdef __cplusplus
}
#endif