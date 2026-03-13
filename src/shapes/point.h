#pragma once

#include "gfxmath.h"
#include "shapes.h"

template <typename T>
concept numeric = requires(T t) 
{ 
    std::is_arithmetic_v<T>; (int32_t)t; 
};

struct point
{
    int32_t x;
    int32_t y;
   
    /**
     * @brief Construct a new empty point object
     */
    point(): x{0}, y{0} {}

    /**
     * @brief Construct a new point object
     * @param x X coordinate of the point
     * @param y Y coordinate of the point
     */
    point(numeric auto x, numeric auto y): x{(int32_t)x}, y{(int32_t)y} {}

    /**
     * @brief Calculate the distance between two points
     * @param other The other point
     * @return uint32_t The distance between the two points
    */
    int32_t distance(point other)
    {
        return isqrt(ipow(this->x - other.x, 2) + ipow(this->y - other.y, 2));
    }

    /**
     * @brief Clamp the point to limits
     * @param min Minimum point
     * @param max Maximum point
     * @return point The clamped point
     */
    point clamp(point min, point max)
    {
        // Clamp the point
        int32_t x = imax(min.x, imin(this->x, max.x));
        int32_t y = imax(min.y, imin(this->y, max.y));

        // Return the new point
        return point(x, y);
    }

    /**
     * @brief Get the minimum of two points
     * @param a First point
     * @param b Second point
     * @return point The minimum of the two points
     */
    static point min(point a, point b)
    {
        return point(imin(a.x, b.x), imin(a.y, b.y));
    }

    /**
     * @brief Get the maximum of two points
     * @param a First point
     * @param b Second point
     * @return point The maximum of the two points
     */
    static point max(point a, point b)
    {
        return point(imax(a.x, b.x), imax(a.y, b.y));
    }

    /**
     * @brief Move the point by an amount
     * @param amountX Amount to move the point on the X axis
     * @param amountY Amount to move the point on the Y axis
     * @return point The moved point
     */
    point move(int32_t amountX, int32_t amountY)
    {
        return point(this->x + amountX, this->y + amountY);
    }

    /**
     * @brief Linearly interpolate between two points
     * @param a First point
     * @param b Second point
     * @param t Interpolation factor
     * @return point The interpolated point
     */
    static point lerp(point a, point b, int32_t t)
    {
        return point(
            (1 - t) * a.x + t * b.x,
            (1 - t) * a.y + t * b.y
        );
    }

    /**
     * @brief Rotate a point around another point
     * @param center Center of rotation
     * @param angle Angle of rotation
     * @return point The rotated point
     * @warning Currently extremely inaccurate and riddled with bugs
     */
    point rotate(point c, int32_t angle)
    {
        int32_t dx = this->x - c.x;
        int32_t dy = this->y - c.y;

        int32_t cos = icos(angle);
        int32_t sin = isin(angle);

        int32_t x = ((dx * cos - dy * sin) + (1 << (SIN_MULTIPLIER_BITS - 1))) >> SIN_MULTIPLIER_BITS;
        int32_t y = ((dx * sin + dy * cos) + (1 << (SIN_MULTIPLIER_BITS - 1))) >> SIN_MULTIPLIER_BITS;
        x += c.x;
        y += c.y;

        return point((int32_t)x, (int32_t)y);
    }

    /**
     * @brief Linear interpolation between two points
     * @param a First point
     * @param b Second point
     * @param t Interpolation factor
     * @return point The interpolated point
     */
    static point lerp(point a, point b, float t)
    {
        return point(
            (1 - t) * a.x + t * b.x,
            (1 - t) * a.y + t * b.y
        );
    }

    /**
     * @brief Get the angle between two points
     * @param a First point
     * @param b Second point
     * @return float The angle between the two points
     */
    static float angle(point a, point b)
    {
        return atan2(b.y - a.y, b.x - a.x);
    }

    /**
     * @brief Find the midpoint between two points
     * @param a First point
     * @param b Second point
     * @return point The midpoint between the two points
     */
    static point midpoint(point a, point b)
    {
        return point((a.x + b.x) / 2, (a.y + b.y) / 2);
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

    point operator<<(uint32_t shift)
    {
        return point(this->x << shift, this->y << shift);
    }
    
    point operator>>(uint32_t shift)
    {
        return point(this->x >> shift, this->y >> shift);
    }

    point operator<<=(uint32_t shift)
    {
        this->x <<= shift;
        this->y <<= shift;
        return *this;
    }

    point operator>>=(uint32_t shift)
    {
        this->x >>= shift;
        this->y >>= shift;
        return *this;
    }

    static inline void swap(point& a, point& b)
    {
        point temp = a;
        a = b;
        b = temp;
    }
};