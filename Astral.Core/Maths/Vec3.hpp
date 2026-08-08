#pragma once
#define VEC3_H
#include "Maths/Vec2.hpp"
#include "Maths/simd.h"

namespace Maths
{
    struct Vec3
    {
        float X;
        float Y;
        float Z;

        inline Vec3()
        {
            X = 0.0f;
            Y = 0.0f;
            Z = 0.0f;
        }
        inline Vec3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }
        inline Vec3(Vec2 vec2, float z)
        {
            X = vec2.X;
            Y = vec2.Y;
            Z = z;
        }
        inline Vec3(float component)
        {
            X = component;
            Y = component;
            Z = component;
        }
        inline Vec3(vec3 cvec)
        {
            X = cvec.X;
            Y = cvec.Y;
            Z = cvec.Z;
        }
        inline void operator+=(const Vec3 other)
        {
            X += other.X;
            Y += other.Y;
            Z += other.Z;
        }
        inline void operator-=(const Vec3 other)
        {
            X -= other.X;
            Y -= other.Y;
            Z -= other.Z;
        }
        inline void operator*=(const Vec3 other)
        {
            X *= other.X;
            Y *= other.Y;
            Z *= other.Z;
        }
        inline void operator/=(const Vec3 other)
        {
            X /= other.X;
            Y /= other.Y;
            Z /= other.Z;
        }
        inline void operator*=(float amount)
        {
            X *= amount;
            Y *= amount;
            Z *= amount;
        }
        inline void operator/=(float amount)
        {
            float oneOver = 1.0f / amount;
            X *= oneOver;
            Y *= oneOver;
            Z *= oneOver;
        }

        #include "Maths/Vec3Impl_Default.inc"

        inline float LengthSquared() const
        {
            return sqrtf(Length());
        }
        inline Vec2 ToVector2() const
        {
            return Maths::Vec2(X, Y);
        }
        inline vec3 ToCVec() const
        {
            return {X, Y, Z};
        }
        inline void Normalize()
        {
            float oneOverLength = 1.0f / sqrtf(X * X + Y * Y + Z * Z);
            X *= oneOverLength;
            Y *= oneOverLength;
            Z *= oneOverLength;
        }
        inline Vec3 Normalized() const
        {
            return Vec3(X, Y, Z) / Length();
        }
        static inline bool AlmostEqual(Vec3 A, Vec3 B, float epsilon = 0.01f)
        {
            return (B - A).Abs() <= Vec3(epsilon);
        }
        static inline float Distance(const Vec3 A, const Vec3 B)
        {
            Vec3 diff = B - A;
            return sqrtf(Dot(diff, diff));
        }
        static inline float DistanceSquared(const Vec3 A, const Vec3 B)
        {
            Vec3 diff = B - A;
            return Dot(diff, diff);
        }
        static inline Vec3 Cross(const Vec3 vec1, const Vec3 vec2)
        {
            return Vec3(
                vec1.Y * vec2.Z - vec1.Z * vec2.Y,
                vec1.Z * vec2.X - vec1.X * vec2.Z,
                vec1.X * vec2.Y - vec1.Y * vec2.X);
        }
    };
}