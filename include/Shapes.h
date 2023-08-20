#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "GfxMath.h"

struct Point
{
    int x;
    int y;

    /**
     * @brief Construct a new empty Point object
    */
    Point()
    {
        this->x = 0;
        this->y = 0;
    }

    /**
     * @brief Construct a new Point object
     * @param x X coordinate of the point
     * @param y Y coordinate of the point
    */
    Point(unsigned int x, unsigned int y)
    {
        this->x = (int)x;
        this->y = (int)y;
    }

    /**
     * @brief Construct a new Point object
     * @param x X coordinate of the point
     * @param y Y coordinate of the point
    */
    Point(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    /**
     * @brief Construct a new Point object
     * @param x X coordinate of the point
     * @param y Y coordinate of the point
    */
    Point(unsigned int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    /**
     * @brief Construct a new Point object
     * @param x X coordinate of the point
     * @param y Y coordinate of the point
    */
    Point(int x, unsigned int y)
    {
        this->x = x;
        this->y = y;
    }

    /**
     * @brief Calculate the distance between two points
     * @param other The other point
     * @return unsigned int The distance between the two points
    */
    unsigned int Distance(Point other)
    {
        return isqrt(ipow(this->x - other.x, 2) + ipow(this->y - other.y, 2));
    }

    /**
     * @brief Get a point on a circle
     * @param radius Radius of the circle
     * @param angle Angle of the point on the circle, in degrees
     * @return Point The point on the circle
    */
    Point getPointOnCircle(int radius, int angle)
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
        return Point(x, y);
    }

    bool operator==(const Point& other)
    {
        return this->x == other.x && this->y == other.y;
    }

    bool operator!=(const Point& other)
    {
        return !(*this == other);
    }

    Point operator+(const Point& other)
    {
        return Point(this->x + other.x, this->y + other.y);
    }

    Point operator-(const Point& other)
    {
        return Point(this->x - other.x, this->y - other.y);
    }

    Point operator*(const Point& other)
    {
        return Point(this->x * other.x, this->y * other.y);
    }

    Point operator/(const Point& other)
    {
        return Point(this->x / other.x, this->y / other.y);
    }
};

struct Rect
{
private:
    Point x;
    Point y;

    unsigned int width;
    unsigned int height;

    unsigned int bottom;
    unsigned int left;
    unsigned int right;
    unsigned int top;

public:
    /**
     * @brief Construct a new empty Rect object
    */
    Rect()
    {
        this->x = Point();
        this->y = Point();
        this->width = 0;
        this->height = 0;
        this->bottom = 0;
        this->left = 0;
        this->right = 0;
        this->top = 0;
    }

    /**
     * @brief Construct a new Rect object
     * @param corner1 Upper left corner of the Rect
     * @param corner2 Lower right corner of the Rect
    */
    Rect(Point corner1, Point corner2)
    {
        // Set the corners of the Rect
        this->x = Point(corner1.x, corner2.y);
        this->y = Point(corner2.x, corner1.y);
        // Set the other variables of the Rect
        this->width = corner2.x - corner1.x;
        this->height = corner2.y - corner1.y;
        this->bottom = this->y.y;
        this->left = this->x.x;
        this->right = this->y.x;
        this->top = this->x.y;
    }

    /**
     * @brief Construct a new Rect object
     * @param center Center of the Rect
     * @param width Width of the Rect
     * @param height Height of the Rect
     * @note The Rect is constructed with the center as the center of the Rect
    */
    Rect(Point center, unsigned int width, unsigned int height)
    {
        // Calculate the corners of the Rect
        this->x = Point(center.x - (width / 2), center.y - (height / 2));
        this->y = Point(center.x + (width / 2), center.y + (height / 2));
        // Set the other variables of the Rect
        this->width = width;
        this->height = height;
        this->bottom = this->y.y;
        this->left = this->x.x;
        this->right = this->y.x;
        this->top = this->x.y;
    }

    /**
     * @brief Get the center of the Rect
     * @return Point
    */
    Point GetCenter()
    {
        return Point(
            (x.x + y.x) / 2,
            (x.y + y.y) / 2
        );
    }

    /**
     * @brief Get the first corner of the Rect
     * @return Point
    */
    Point X()
    {
        return x;
    }

    /**
     * @brief Get the second corner of the Rect
     * @return Point
    */
    Point Y()
    {
        return y;
    }

    /**
     * @brief Get the width of the Rect
     * @return unsigned int
    */
    unsigned int Width()
    {
        return width;
    }

    /**
     * @brief Get the height of the Rect
     * @return unsigned int
    */
    unsigned int Height()
    {
        return height;
    }

    /**
     * @brief Get the bottom of the Rect
     * @return unsigned int that is the Y coordinate of the bottom of the Rect
    */
    unsigned int Bottom()
    {
        return bottom;
    }

    /**
     * @brief Get the left of the Rect
     * @return unsigned int that is the X coordinate of the left of the Rect
    */
    unsigned int Left()
    {
        return left;
    }

    /**
     * @brief Get the right of the Rect
     * @return unsigned int that is the X coordinate of the right of the Rect
    */
    unsigned int Right()
    {
        return right;
    }

    /**
     * @brief Get the top of the Rect
     * @return unsigned int that is the Y coordinate of the top of the Rect
    */
    unsigned int Top()
    {
        return top;
    }
};

inline void swap(Point& a, Point& b)
{
	Point temp = a;
	a = b;
	b = temp;
}

#ifdef __cplusplus
}
#endif