#include "gauge.h"

/**
 * @brief Construct a new Dials object
 * @param frameBuffer Frame buffer to draw on
 * @param screenWidth Width of the screen
 * @param screenHeight Height of the screen
 * @param center Center of the dial
 * @param radius Radius of the dial
 * @param minValue Minimum value of the dial
 * @param maxValue Maximum value of the dial
 * @param initValue Initial value of the dial
 * @param valueColors colors to use for the dial
 * @param numberOfValueColors Number of colors to use for the dial
 * @param type Type of dial to draw
 * @param angle Dial angle, represents how much of a circle the dial should be. Default is 230 degrees
 * @note The dial will be drawn on the frame buffer
*/
dialGauge::dialGauge(graphics* graphics_ptr, int32_t screenWidth, int32_t screenHeight, point center, int32_t radius,
	int32_t minValue, int32_t maxValue, color* valueColors, size_t numberOfValueColors, dialGaugeType_t type, int32_t angle)
{
	// Copy the display parameters to the object
	this->graphics_ptr = graphics_ptr;
	this->width = screenWidth;
	this->height = screenHeight;

	// Copy the dial parameters to the object
	this->center = center;
	this->angle = angle;
	this->halfAngle = angle / 2;
	this->radius = radius;
	this->minValue = minValue;
	this->maxValue = maxValue;
	this->valueColors = valueColors;
	this->numberOfValueColors = numberOfValueColors;
	this->type = type;
}

/**
 * @brief Set the color of the needle
 * @param value color to set the needle to
 */
void dialGauge::setNeedleColor(color value)
{
	this->needleColor = value;
}

/**
 * @brief Set the background color of the dial
 * @param value color to set the background to
 */
void dialGauge::attachBackgroundColor(color value)
{
	if (this->hasBackground) return;
	this->background = value;
	this->hasBackground = true;
}

/**
 * @brief Draw a line across the dial gauge to indicate stops/starts/set points etc
 * @param value Value to draw the line at
 * @param width Width of the line
 * @param color color to draw the line in
 */
void dialGauge::drawLine(int32_t value, int32_t width, color color)
{
	// Make sure the thickness is above 0 
	if (width <= 0) width = 1;
	// Find the angle based of the value
	int32_t angle = (int32_t)((value * this->angle) / this->maxValue) - this->halfAngle;
	// Clamp the angle to the min and max angles
	angle = imin(angle, this->halfAngle);
	angle = imax(angle, -this->halfAngle);
	// Offset the angle by 90 degrees
	angle -= 90;
	// Calculate the start and end points of the line by finding the point on the circle
	point start = this->center;
	start = this->getPointOnCircle(start, this->innerRadius, angle);
	point end = this->center;
	end = this->getPointOnCircle(end, this->radius, angle);
	// Draw the line
	this->graphics_ptr->drawLineAntiAliased(start, end, color);
}

/**
 * @brief Update the dial value
 * @param value New value to set the dial to
 */
void dialGauge::update(int32_t value)
{
	// Draw the dial
	switch (this->type)
	{
	case dialGaugeType_t::DialSimple:
		// Draw the dial
		this->drawSimpleDial(value);
		// Draw the needle
		this->drawNeedle(value);
		break;
	case dialGaugeType_t::DialSimple2:
		// Draw the dial
		this->drawSimpleDial2(value);
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
void dialGauge::drawNeedle(int32_t value)
{
	// Calculate the needle radius
	int32_t needleRadius = (int32_t)(this->radius * 0.05);
	// Calculate the angle of the needle
	int32_t needleAngle = (int32_t)((value * this->angle) / this->maxValue) - this->halfAngle;
	// Clamp the needle angle to the dial angle
	needleAngle = imin(needleAngle, this->halfAngle);
	needleAngle = imax(needleAngle, -this->halfAngle);
	// Offset it by 90 degrees to start at the top of the dial
	needleAngle -= 90;

	// Find the sin and cos of the needle angle with integer math
	int32_t cosValue = this->radius * icos(needleAngle);
	cosValue >>= FIXED_POINT_SCALE_BITS;
	int32_t sinValue = this->radius * isin(needleAngle);
	sinValue >>= FIXED_POINT_SCALE_BITS;
	// Calculate the end point of the needle
	point endPoint;
	endPoint.x = this->center.x + cosValue;
	endPoint.y = this->center.y + sinValue;

	// Draw the needle
	this->graphics_ptr->drawFilledCircle(this->center, needleRadius, this->needleColor);

	// Draw the needle line as a line if the needleRadius is less than 4 otherwise draw it as a wedge
	if (needleRadius < 4)
		this->graphics_ptr->drawLine(this->center, endPoint, this->needleColor);
	else
	{
		// Calculate the angle of the base
		int32_t baseAngle1 = needleAngle + 90; // Add 90 degrees
		if (baseAngle1 >= 360) baseAngle1 -= 360;

		int32_t baseAngle2 = needleAngle - 90; // Subtract 90 degrees
		if (baseAngle2 < 0) baseAngle2 += 360;

		// Calculate the offset points
		int32_t x1 = needleRadius * icos(baseAngle1);
		x1 >>= FIXED_POINT_SCALE_BITS;
		int32_t y1 = needleRadius * isin(baseAngle1);
		y1 >>= FIXED_POINT_SCALE_BITS;
		point p1 = { center.x + x1, center.y + y1 };

		// Do the same for the other point
		int32_t x2 = needleRadius * icos(baseAngle2);
		x2 >>= FIXED_POINT_SCALE_BITS;
		int32_t y2 = needleRadius * isin(baseAngle2);
		y2 >>= FIXED_POINT_SCALE_BITS;
		point p2 = { center.x + x2, center.y + y2 };

		// Draw the needle
		point points[] = { p1, p2, endPoint };
		this->graphics_ptr->drawFilledPolygon(points, 3, this->needleColor);
	}
}

/**
 * @private 
 * @brief Draw a simple dial
 * @param value Value to draw the dial at
 */
void dialGauge::drawSimpleDial(int32_t value)
{
	// Calculate the inner radius of the dial by multiplying with the golden ratio
	this->innerRadius = (int32_t)(this->radius * 0.618);
	// Divide the dial into segments
	int32_t segmentAngle = this->angle / this->numberOfValueColors;
	// Verify that the segmentAngle adds up to the dial angle
	int32_t error = this->angle - (segmentAngle * this->numberOfValueColors);
	int32_t errorHalf = 0;
	// Divide the error by 2 if its above 1, otherwise set it to 1
	if (error > 1) errorHalf = error >> 1;
	else if (error == 1) errorHalf = 1;

	// Draw the background
	if(this->hasBackground)
		this->graphics_ptr->drawFilledCircle(this->center, this->radius, this->background);

	// Draw the segments
	int32_t offset = 0;
	for (int32_t i = 0; i < this->numberOfValueColors; i ++)
	{
		int32_t angle = (segmentAngle * i) + offset;
		int32_t angleTarget = segmentAngle;
		// We need to offset the angle by half the dial angle in degrees, since we start at 0 degrees
		angle -= this->halfAngle;
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
		this->graphics_ptr->drawFilledDualArc(this->center, this->innerRadius, this->radius, angle, angleTarget, this->valueColors[i]);
	}
}

/**
 * @private
 * @brief Draw a simple dial, with a different style
 * @param value Value to draw the dial at
 */
void dialGauge::drawSimpleDial2(int32_t value)
{
	// Make the dial's inner radius about 85% of the outer radius
	this->innerRadius = (int32_t)(this->radius * 0.85);
	int32_t pointerThickness = (int32_t)(this->radius * 0.05);

	// We only need two colors for this dial
	if (this->numberOfValueColors < 2) return;

	// Clamp the min and max values
	value = imin(value, this->maxValue);
	value = imax(value, this->minValue);
	// Find the angle of the value in relation to the dial angle, mapped to the min/max value
	int32_t angle = (int32_t)((value * this->angle) / this->maxValue) - this->halfAngle;
	// Clamp the angle to the dial angle
	angle = imin(angle, this->halfAngle);
	angle = imax(angle, -this->halfAngle);
	// Offset it by 90 degrees to start at the top of the dial
	angle -= 90;

	// Calculate the actual angles we will use
	int32_t angle1Start = (-this->halfAngle) - 90;
	int32_t angle1End = angle - pointerThickness >> 1;
	int32_t angle2Start = angle + pointerThickness >> 1;
	int32_t angle2End = this->halfAngle - 90;

	// Draw the dual filled arc
	this->graphics_ptr->drawFilledDualArc(this->center, this->innerRadius, this->radius, angle1Start, angle1End, this->valueColors[0]);
	this->graphics_ptr->drawFilledDualArc(this->center, this->innerRadius, this->radius, angle1End, angle2Start, this->valueColors[1]);
	this->graphics_ptr->drawFilledDualArc(this->center, this->innerRadius, this->radius, angle2Start, angle2End, this->valueColors[0]);
}

/**
 * @private
 * @brief Get a point on a circle
 * @param radius Radius of the circle
 * @param angle Angle of the point on the circle, in degrees
 * @return point The point on the circle
*/
point dialGauge::getPointOnCircle(point p, int32_t radius, int32_t angle)
{
	// Calculate the point on the circle
	int32_t x = (radius * icos(angle));
	x >>= FIXED_POINT_SCALE_BITS;
	x += p.x;
	int32_t y = (radius * isin(angle));
	y >>= FIXED_POINT_SCALE_BITS;
	y += p.y;

	// Return the point
	return point(x, y);
}