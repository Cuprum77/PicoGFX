#include "gfxmath.h"

int32_t cosTable[NUMBER_OF_ANGLES];
int32_t sinTable[NUMBER_OF_ANGLES];
int32_t tanTable[NUMBER_OF_ANGLES];
int32_t atanTable[NUMBER_OF_ANGLES];

int32_t imin(int32_t x, int32_t y)
{
	return (x < y) ? x : y;
}

int32_t imax(int32_t x, int32_t y)
{
	return (x > y) ? x : y;
}

int32_t iabs(int32_t x)
{
	return (x < 0) ? -x : x;
}

int32_t isqrt(int32_t x)
{
	int32_t result = 0;
	
	while ((result + 1) * (result + 1) <= x)
		result++;
	
	return result;
}

int32_t ipow(int32_t x, int32_t y)
{
	int32_t result = 1;

	for (int32_t i = 0; i < y; i++)
		result *= x;

	return result;
}

int32_t ifactorial(int32_t x)
{
	int32_t result = 1;

	for (int32_t i = 1; i <= x; i++)
		result *= i;

	return result;
}

void fillLookupTables()
{
	for (int32_t angle = 0; angle < NUMBER_OF_ANGLES; ++angle)
	{
		// Convert angle to radians
		double radians = angle * PI / (NUMBER_OF_ANGLES / 2);

		// Calculate cosine and sine using math functions
		double cosValue = cos(radians);
		double sinValue = sin(radians);
		double tanValue = tan(radians);
		double atanValue = atan(radians);

		// Convert to fixed-point representation
		cosTable[angle] = (int32_t)(cosValue * FIXED_POINT_SCALE);
		sinTable[angle] = (int32_t)(sinValue * FIXED_POINT_SCALE);
		tanTable[angle] = (int32_t)(tanValue * FIXED_POINT_SCALE);
		atanTable[angle] = (int32_t)(atanValue * FIXED_POINT_SCALE);
	}
}

void pointOnCircle(int32_t radius, int32_t angle, int32_t offsetX, int32_t offsetY, int32_t* x, int32_t* y)
{
	int32_t angleX = cosTable[angle * ANGLE_SCALE];
	int32_t angleY = sinTable[angle * ANGLE_SCALE];

	// multiply with the radius
	angleX *= radius;
	angleY *= radius;

	// divide by the scaler
	angleX /= FIXED_POINT_SCALE;
	angleY /= FIXED_POINT_SCALE;

	// offset it
	angleX += offsetX;
	angleY -= offsetY;

	// absolute it as we cannot have negative pixels!
	*x = iabs(angleX);
	*y = iabs(angleY);
}

int32_t lerp(int32_t v0, int32_t v1, int32_t t)
{
	return (1 - t) * v0 + t * v1;
}

int32_t clamp(int32_t x, int32_t min, int32_t max)
{
	// clamp x to min
	if (x < min) x = min;
	// clamp x to max
	if (x > max) x = max;
	// return clamped value
	return x;
}