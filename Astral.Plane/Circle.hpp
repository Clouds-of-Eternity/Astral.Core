#pragma once
#include "Maths/Vec2.hpp"

struct Circle
{
    Maths::Vec2 center;
    float radius;

    inline Circle()
    {
        center = Maths::Vec2();
        radius = 0.0f;
    }
    inline Circle(Maths::Vec2 center, float radius)
    {
        this->center = center;
        this->radius = radius;
    }
    inline Circle(float x, float y, float radius)
    {
        this->center = Maths::Vec2(x, y);
        this->radius = radius;
    }

    inline bool Contains(Maths::Vec2 point) const
    {
        return Maths::Vec2::DistanceSquared(center, point) <= radius * radius;
    }
    inline bool Intersects(Circle other) const
    {
        float totalDist = radius + other.radius;
        return Maths::Vec2::DistanceSquared(center, other.center) <= totalDist * totalDist;
    }
};