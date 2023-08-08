#include "Gauge.hpp"

/**
 * @brief Construct a new Dials object
 * @param frameBuffer Frame buffer to draw on
 * @param params Display parameters
 * @param center Center of the dial
 * @param radius Radius of the dial
 * @param minValue Minimum value of the dial
 * @param maxValue Maximum value of the dial
 * @param initValue Initial value of the dial
 * @param valueColors Colors to use for the dial
 * @param numberOfValueColors Number of colors to use for the dial
 * @param type Type of dial to draw
 * @note The dial will be drawn on the frame buffer
*/
DialGauge::DialGauge(Graphics* graphics, Display_Params params, Point center, int radius, int minValue,
			int maxValue, int initValue, Color* valueColors, size_t numberOfValueColors, DialGaugeType_t type)
{
	// Copy the display parameters to the object
	this->graphics = graphics;
	this->width = params.width;
	this->height = params.height;
	this->totalPixels = (size_t)(params.width) * (size_t)(params.height);

	// Copy the dial parameters to the object
	this->center = center;
	this->radius = radius;
	this->minValue = minValue;
	this->maxValue = maxValue;
	this->valueColors = valueColors;
	this->numberOfValueColors = numberOfValueColors;
	this->type = type;

	// Draw the dial
	this->update(initValue);
}

/**
 * @brief Set the color of the needle
 * @param value Color to set the needle to
 */
void DialGauge::setNeedleColor(Color value)
{
	this->needleColor = value;
}

/**
 * @brief Set the background color of the dial
 * @param value Color to set the background to
 */
void DialGauge::attachBackgroundColor(Color value)
{
	if (this->hasBackground) return;
	this->background = value;
	this->hasBackground = true;
}

/**
 * @brief Update the dial value
 * @param value New value to set the dial to
 */
void DialGauge::update(int value)
{
	// Draw the dial
	switch (this->type)
	{
	case DialGaugeType_t::DialSimple:
		// Draw the dial
		this->drawSimpleDial(value);
		// Draw the needle
		this->drawNeedle(value);
		break;
	default:
		break;
	}
}


/**
 * @private
 * @brief Draw a needle
 * @param value Value to draw the dial at
 */
void DialGauge::drawNeedle(int value)
{
	// Define the angle of the dial
	static const int halfDialAngle = DIAL_ANGLE >> 1;
	// Calculate the needle radius
	int needleRadius = (int)(this->radius * 0.05);

	// Calculate the angle of the needle
	int needleAngle = (int)((value * DIAL_ANGLE) / this->maxValue) - halfDialAngle;
	// Clamp the needle angle to the dial angle
	needleAngle = imin(needleAngle, halfDialAngle);
	needleAngle = imax(needleAngle, -halfDialAngle);
	// Offset it by 90 degrees to start at the top of the dial
	needleAngle -= 90;
	// Multiply the angle by 10 as the sin and cos tables have a resolution of 0.1 degrees
	needleAngle *= 10;

	// Normalize it to within [0, 3600]
	needleAngle = needleAngle + 3600;
	while (needleAngle >= 3600) needleAngle -= 3600;

	// Find the sin and cos of the needle angle with integer math
	int cosValue = this->radius * cosTable[needleAngle];
	cosValue >>= FIXED_POINT_SCALE_BITS;
	int sinValue = this->radius * sinTable[needleAngle];
	sinValue >>= FIXED_POINT_SCALE_BITS;
	// Calculate the end point of the needle
	Point endPoint;
	endPoint.x = this->center.x + cosValue;
	endPoint.y = this->center.y + sinValue;

	// Draw the needle
	this->graphics->drawFilledCircle(this->center, needleRadius, this->needleColor);

	// Draw the needle line as a line if the needleRadius is less than 4 otherwise draw it as a wedge
	if (needleRadius < 4)
		this->graphics->drawLine(this->center, endPoint, this->needleColor);
	else
	{
		// Calculate the angle of the base
		int baseAngle1 = needleAngle + 900; // Add 90 degrees
		if (baseAngle1 >= 3600) baseAngle1 -= 3600;

		int baseAngle2 = needleAngle - 900; // Subtract 90 degrees
		if (baseAngle2 < 0) baseAngle2 += 3600;

		// Calculate the offset points
		int x1 = needleRadius * cosTable[baseAngle1];
		x1 >>= FIXED_POINT_SCALE_BITS;
		int y1 = needleRadius * sinTable[baseAngle1];
		y1 >>= FIXED_POINT_SCALE_BITS;
		Point p1 = { center.x + x1, center.y + y1 };

		// Do the same for the other point
		int x2 = needleRadius * cosTable[baseAngle2];
		x2 >>= FIXED_POINT_SCALE_BITS;
		int y2 = needleRadius * sinTable[baseAngle2];
		y2 >>= FIXED_POINT_SCALE_BITS;
		Point p2 = { center.x + x2, center.y + y2 };

		// Draw the needle
		Point points[] = { p1, p2, endPoint };
		this->graphics->drawFilledPolygon(points, 3, this->needleColor);
	}
}

/**
 * @private 
 * @brief Draw a simple dial
 * @param value Value to draw the dial at
 */
void DialGauge::drawSimpleDial(int value)
{
	// Define the angle of the dial
	static const int halfDialAngle = DIAL_ANGLE >> 1;
	// Calculate the inner radius of the dial using the golden ratio
	int innerRadius = (int)(this->radius * 0.618);
	// Divide the dial into segments
	int segmentAngle = DIAL_ANGLE / this->numberOfValueColors;
	// Verify that the segmentAngle adds up to the dial angle
	int error = DIAL_ANGLE - (segmentAngle * this->numberOfValueColors);
	int errorHalf = 0;
	// Divide the error by 2 if its above 1, otherwise set it to 1
	if (error > 1) errorHalf = error >> 1;
	else if (error == 1) errorHalf = 1;

	// Draw the background
	if(this->hasBackground)
		this->graphics->drawFilledCircle(this->center, this->radius, this->background);

	// Draw the segments
	int offset = 0;
	for (int i = 0; i < this->numberOfValueColors; i ++)
	{
		int angle = (segmentAngle * i) + offset;
		int angleTarget = segmentAngle;
		// We need to offset the angle by half the dial angle in degrees, since we start at 0 degrees
		angle -= halfDialAngle;
		// Subtract an additional 90 degrees to start at the top of the dial, this is because a circle starts at 3 o'clock
		angle -= 90;
		// Add the angle to the target
		angleTarget += angle;

		// Handle edge case where error is not 0
		if (error != 0)
		{
			// Error here represents degrees that need to be added to the segment angle
			if ((i == 0 || i == (this->numberOfValueColors - 1)) && error > 1)
			{
				// Add the error to the start angle
				angleTarget += errorHalf;
				offset = errorHalf;
				// Subtract the error from the error counter
				error -= errorHalf;
			}
		}
		// Draw the segment
		this->graphics->drawFilledDualArc(this->center, innerRadius, this->radius, angle, angleTarget, this->valueColors[i]);
	}
}