#pragma once
#include "Maths/Vec3.hpp"

struct BoundingBox
{
    Maths::Vec3 center;
    Maths::Vec3 extents;

    inline Maths::Vec3 GetMin() const
    {
        return center - extents;
    }
    inline Maths::Vec3 GetMax() const
    {
        return center + extents;
    }
    inline Maths::Vec3 GetTotalSize() const
    {
        return extents * 2.0f;
    }
    inline void SetTotalSize(Maths::Vec3 sz)
    {
        extents = sz * 0.5f;
    }
    inline float GetWidth() const
    {
        return extents.X * 2.0f;
    }
    inline float GetHeight() const
    {
        return extents.Y * 2.0f;
    }
    inline float GetDepth() const
    {
        return extents.Z * 2.0f;
    }
    inline void SetWidth(float value)
    {
        extents.X = value * 0.5f;
    }
    inline void SetHeight(float value)
    {
        extents.Y = value * 0.5f;
    }
    inline void SetDepth(float value)
    {
        extents.Z = value * 0.5f;
    }
    inline bool Contains(Maths::Vec3 position) const
    {
        return position < GetMax() && position > GetMin();
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