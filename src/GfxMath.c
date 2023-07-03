#include "GfxMath.h"

int cosTable[NUMBER_OF_ANGLES];
int sinTable[NUMBER_OF_ANGLES];
int tanTable[NUMBER_OF_ANGLES];

int imin(int x, int y)
{
	return (x < y) ? x : y;
}

int imax(int x, int y)
{
	return (x > y) ? x : y;
}

int iabs(int x)
{
	return (x < 0) ? -x : x;
}

unsigned int isqrt(unsigned int x)
{
	unsigned int result = 0;
	
	while ((result + 1) * (result + 1) <= x)
		result++;
	
	return result;
}

unsigned int ipow(unsigned int x, unsigned int y)
{
	unsigned int result = 1;

	for (unsigned int i = 0; i < y; i++)
		result *= x;

	return result;
}

unsigned int ifactorial(unsigned int x)
{
	unsigned int result = 1;

	for (unsigned int i = 1; i <= x; i++)
		result *= i;

	return result;
}

void fillLookupTables()
{
	for (int angle = 0; angle < NUMBER_OF_ANGLES; ++angle)
	{
		// Convert angle to radians
		double radians = angle * PI / NUMBER_OF_ANGLES / 2;

		// Calculate cosine and sine using math functions
		double cosValue = cos(radians);
		double sinValue = sin(radians);

		// Convert to fixed-point representation
		cosTable[angle] = (int)(cosValue * FIXED_POINT_SCALE);
		sinTable[angle] = (int)(sinValue * FIXED_POINT_SCALE);
	}
}