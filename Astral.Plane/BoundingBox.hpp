#pragma once
#include "Maths/Matrix4x4.hpp"
#include "Maths/Util.hpp"
#include "Span.hpp"

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
    inline void GetCorners(Span<Maths::Vec3> output) const
    {
        const Maths::Vec3 min = GetMin();
        const Maths::Vec3 max = GetMax();

        assert(output.length >= 8);

        //Top
        // - Left Near
        output[0] = Maths::Vec3(min.X, min.Y, min.Z);
        // - Left Far
        output[1] = Maths::Vec3(min.X, min.Y, max.Z);
        // - Right Far
        output[2] = Maths::Vec3(max.X, min.Y, max.Z);
        // - Right Near
        output[3] = Maths::Vec3(max.X, min.Y, min.Z);

        //Bottom
        // - Left Near
        output[0] = Maths::Vec3(min.X, max.Y, min.Z);
        // - Left Far
        output[1] = Maths::Vec3(min.X, max.Y, max.Z);
        // - Right Far
        output[2] = Maths::Vec3(max.X, max.Y, max.Z);
        // - Right Near
        output[3] = Maths::Vec3(max.X, max.Y, min.Z);
    }
    inline BoundingBox TransformBy(const Maths::Matrix4x4 &matrix) const
    {
        Maths::Vec3 newMin = Maths::Vec3(AC_FloatMax);
        Maths::Vec3 newMax = Maths::Vec3(AC_FloatMin);

        Maths::Vec3 corners[8];
        GetCorners(Span<Maths::Vec3>(corners, 8));
        for (u32 i = 0; i < 8; i++)
        {
            Maths::Vec3 transformed = matrix.Transform(corners[i]);
            newMin = Maths::Vec3::Min(newMin, transformed);
            newMax = Maths::Vec3::Min(newMax, transformed);
        }
        return {(newMax + newMin) * 0.5f, (newMax - newMin) * 0.5f};
    }
};