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
    unsigned int Distance(point other)
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
    point x;
    point y;

    unsigned int width;
    unsigned int height;

    unsigned int bottom;
    unsigned int left;
    unsigned int right;
    unsigned int top;

public:
    /**
     * @brief Construct a new empty rect object
    */
    rect()
    {
        this->x = point();
        this->y = point();
        this->width = 0;
        this->height = 0;
        this->bottom = 0;
        this->left = 0;
        this->right = 0;
        this->top = 0;
    }

    /**
     * @brief Construct a new rect object
     * @param corner1 Upper left corner of the rect
     * @param corner2 Lower right corner of the rect
    */
    rect(point corner1, point corner2)
    {
        // Set the corners of the rect
        this->x = point(corner1.x, corner2.y);
        this->y = point(corner2.x, corner1.y);
        // Set the other variables of the rect
        this->width = corner2.x - corner1.x;
        this->height = corner2.y - corner1.y;
        this->bottom = this->y.y;
        this->left = this->x.x;
        this->right = this->y.x;
        this->top = this->x.y;
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
        this->x = point(center.x - (width / 2), center.y - (height / 2));
        this->y = point(center.x + (width / 2), center.y + (height / 2));
        // Set the other variables of the rect
        this->width = width;
        this->height = height;
        this->bottom = this->y.y;
        this->left = this->x.x;
        this->right = this->y.x;
        this->top = this->x.y;
    }

    /**
     * @brief Get the center of the rect
     * @return point
    */
    point GetCenter()
    {
        return point(
            (x.x + y.x) / 2,
            (x.y + y.y) / 2
        );
    }

    /**
     * @brief Get the first corner of the rect
     * @return point
    */
    point X()
    {
        return x;
    }

    /**
     * @brief Get the second corner of the rect
     * @return point
    */
    point Y()
    {
        return y;
    }

    /**
     * @brief Get the width of the rect
     * @return unsigned int
    */
    unsigned int Width()
    {
        return width;
    }

    /**
     * @brief Get the height of the rect
     * @return unsigned int
    */
    unsigned int Height()
    {
        return height;
    }

    /**
     * @brief Get the bottom of the rect
     * @return unsigned int that is the Y coordinate of the bottom of the rect
    */
    unsigned int Bottom()
    {
        return bottom;
    }

    /**
     * @brief Get the left of the rect
     * @return unsigned int that is the X coordinate of the left of the rect
    */
    unsigned int Left()
    {
        return left;
    }

    /**
     * @brief Get the right of the rect
     * @return unsigned int that is the X coordinate of the right of the rect
    */
    unsigned int Right()
    {
        return right;
    }

    /**
     * @brief Get the top of the rect
     * @return unsigned int that is the Y coordinate of the top of the rect
    */
    unsigned int Top()
    {
        return top;
    }
};

inline void swap(point& a, point& b)
{
	point temp = a;
	a = b;
	b = temp;
}