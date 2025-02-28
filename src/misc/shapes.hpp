#pragma once

#include "gfxmath.h"

template <typename T>
concept numeric = requires(T t) 
{ 
    std::is_arithmetic_v<T>; (int)t; 
};

struct point
{
    int x;
    int y;
   
    /**
     * @brief Construct a new empty point object
     */
    point(): x{0}, y{0} {}

    /**
     * @brief Construct a new point object
     * @param x X coordinate of the point
     * @param y Y coordinate of the point
     */
    point(numeric auto x, numeric auto y): x{(int)x}, y{(int)y} {}

    /**
     * @brief Calculate the distance between two points
     * @param other The other point
     * @return unsigned int The distance between the two points
    */
    unsigned int distance(point other)
    {
        return isqrt(ipow(this->x - other.x, 2) + ipow(this->y - other.y, 2));
    }

    /**
     * @brief Get a point on a circle
     * @param radius Radius of the circle
     * @param angle Angle of the point on the circle, in degrees
     * @return point The point on the circle
    */
    point getPointOnCircle(int radius, int angle)
    {
        // Clamp the angle between 0 and 360
        while (angle < 0) angle += 360;
        while (angle > 360) angle -= 360;
        // Multiply the angle by 10
        angle *= 10;

        // Calculate the point on the circle
        int x = (radius * cosTable[angle]);
        x >>= FIXED_POINT_SCALE_BITS;
        x += this->x;
        int y = (radius * sinTable[angle]);
        y >>= FIXED_POINT_SCALE_BITS;
        y += this->y;

        // Return the point
        return point(x, y);
    }

    bool operator==(const point& other)
    {
        return this->x == other.x && this->y == other.y;
    }

    bool operator!=(const point& other)
    {
        return !(*this == other);
    }

    point operator+(const point& other)
    {
        return point(this->x + other.x, this->y + other.y);
    }

    point operator-(const point& other)
    {
        return point(this->x - other.x, this->y - other.y);
    }

    point operator*(const point& other)
    {
        return point(this->x * other.x, this->y * other.y);
    }

    point operator/(const point& other)
    {
        return point(this->x / other.x, this->y / other.y);
    }
};

struct rect
{
private:
    point px;
    point py;

    unsigned int width_val;
    unsigned int height_val;

    unsigned int bottom_val;
    unsigned int left_val;
    unsigned int right_val;
    unsigned int top_val;

public:
    /**
     * @brief Construct a new empty rect object
    */
    rect()
    {
        this->px = point();
        this->py = point();
        this->width_val = 0;
        this->height_val = 0;
        this->bottom_val = 0;
        this->left_val = 0;
        this->right_val = 0;
        this->top_val = 0;
    }

    /**
     * @brief Construct a new rect object
     * @param corner1 Upper left corner of the rect
     * @param corner2 Lower right corner of the rect
    */
    rect(point corner1, point corner2)
    {
        // Set the corners of the rect
        this->px = point(corner1.x, corner2.y);
        this->py = point(corner2.x, corner1.y);
        // Set the other variables of the rect
        this->width_val = corner2.x - corner1.x;
        this->height_val = corner2.y - corner1.y;
        this->bottom_val = this->px.y;
        this->left_val = this->px.x;
        this->right_val = this->py.x;
        this->top_val = this->py.y;
    }

    /**
     * @brief Construct a new rect object
     * @param center Center of the rect
     * @param width Width of the rect
     * @param height Height of the rect
     * @note The rect is constructed with the center as the center of the rect
    */
    rect(point center, unsigned int width, unsigned int height)
    {
        // Calculate the corners of the rect
        this->px = point(center.x - (width / 2), center.y - (height / 2));
        this->py = point(center.x + (width / 2), center.y + (height / 2));
        // Set the other variables of the rect
        this->width_val = width;
        this->height_val = height;
        this->bottom_val = this->px.y;
        this->left_val = this->px.x;
        this->right_val = this->py.x;
        this->top_val = this->py.y;
    }

    /**
     * @brief Construct a new rect object based on two rects
     * @param rect1 First rect
     * @param rect2 Second rect
     * @note This concatenates the two rects into one rect
     */
    rect(rect rect1, rect rect2)
    {
        // Calculate the corners of the rect
        this->px = point(imin(rect1.left(), rect2.left()), imin(rect1.top(), rect2.top()));
        this->py = point(imax(rect1.right(), rect2.right()), imax(rect1.bottom(), rect2.bottom()));
        // Set the other variables of the rect
        this->width_val = this->py.x - this->px.x;
        this->height_val = this->py.y - this->px.y;
        this->bottom_val = this->px.y;
        this->left_val = this->px.x;
        this->right_val = this->py.x;
        this->top_val = this->py.y;
    }

    /**
     * @brief Get the center of the rect
     * @return point
    */
    point getCenter()
    {
        return point(
            (px.x + py.x) / 2,
            (px.y + py.y) / 2
        );
    }

    /**
     * @brief Get the first corner of the rect
     * @return point
    */
    point x()
    {
        return px;
    }

    /**
     * @brief Get the second corner of the rect
     * @return point
    */
    point y()
    {
        return py;
    }

    /**
     * @brief Get the width of the rect
     * @return unsigned int
    */
    unsigned int width()
    {
        return width_val;
    }

    /**
     * @brief Get the height of the rect
     * @return unsigned int
    */
    unsigned int height()
    {
        return height_val;
    }

    /**
     * @brief Get the bottom of the rect
     * @return unsigned int that is the Y coordinate of the bottom of the rect
    */
    unsigned int bottom()
    {
        return bottom_val;
    }

    /**
     * @brief Get the bottom
     * @return point that is the bottom of the rect, middle of the rect
     */
    point bottomPoint()
    {
        return point((left_val + right_val) >> 1, bottom_val);
    }

    /**
     * @brief Get the left of the rect
     * @return unsigned int that is the X coordinate of the left of the rect
    */
    unsigned int left()
    {
        return left_val;
    }

    /**
     * @brief Get the left of the rect
     * @return point that is the left of the rect, middle of the rect
     */
    point leftPoint()
    {
        return point(left_val, (top_val + bottom_val) >> 1);
    }

    /**
     * @brief Get the right of the rect
     * @return unsigned int that is the X coordinate of the right of the rect
    */
    unsigned int right()
    {
        return right_val;
    }

    /**
     * @brief Get the top of the rect
     * @return point that is the right of the rect, middle of the rect
     */
    point rightPoint()
    {
        return point(right_val, (top_val + bottom_val) >> 1);
    }

    /**
     * @brief Get the top of the rect
     * @return unsigned int that is the Y coordinate of the top of the rect
    */
    unsigned int top()
    {
        return top_val;
    }

    /**
     * @brief Get the top of the rect, as a point
     * @return point that is the top of the rect, middle of the rect
     */
    point topPoint()
    {
        return point((left_val + right_val) >> 1, top_val);
    }

    /**
     * @brief Clamp the rect to limits
     * @param min Minimum point
     * @param max Maximum point
     */
    rect clamp(point min, point max)
    {
        // Clamp the sides of the rect
        uint32_t x1 = imax(this->px.x, min.x);
        uint32_t y1 = imax(this->px.y, min.y);
        uint32_t x2 = imin(this->py.x, max.x);
        uint32_t y2 = imin(this->py.y, max.y);

        // Return the new rect
        return rect(point(x1, y1), point(x2, y2));
    }
};

inline void swap(point& a, point& b)
{
	point temp = a;
	a = b;
	b = temp;
}