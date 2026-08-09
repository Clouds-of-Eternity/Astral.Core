#pragma once
#include "Maths/Vec3.hpp"

struct BoundingBox
{
    Maths::Vec3 center;
    Maths::Vec3 extents;

    inline Maths::Vec3 Min() const
    {
        return center - extents;
    }
    inline Maths::Vec3 Max() const
    {
        return center + extents;
    }
    inline bool Intersects(BoundingBox other) const
    {
        Maths::Vec3 min = center - extents;
        Maths::Vec3 max = center + extents;
        Maths::Vec3 otherMin = other.center - other.extents;
        Maths::Vec3 otherMax = other.center - other.extents;

        if ((max.X >= otherMin.X) && (min.X <= otherMax.X))
        {
            if ((max.Y < otherMin.Y) || (min.Y > otherMax.Y))
            {
                return false;
            }

            return (max.Z >= otherMin.Z) && (min.Z <= otherMax.Z);
        }
        return false;
    }
};