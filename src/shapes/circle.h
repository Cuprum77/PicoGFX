#pragma once

#include "shapes.h"

struct circle
{
private:
    point center;
    uint32_t radius;

public:
    circle()
    {
        this->center = point();
        this->radius = 0;
    }

    /**
     * @brief Construct a new circle object
     * @param center Center of the circle
     * @param radius Radius of the circle
     */
    circle(point center, numeric auto radius)
    {
        this->center = center;
        this->radius = (uint32_t)radius;
    }

    /**
     * @brief Get the center of the circle
     * @return point The center of the circle
     */
    point getCenter()
    {
        return this->center;
    }

    /**
     * @brief Get the radius of the circle
     * @return uint32_t The radius of the circle
     */
    uint32_t getRadius()
    {
        return this->radius;
    }

    /**
     * @brief Check if a point is inside the circle
     * @param p Point to check
     * @return bool True if the point is inside the circle
     */
    bool inside(point p)
    {
        return p.distance(this->center) <= this->radius;
    }
};