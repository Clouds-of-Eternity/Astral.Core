#pragma once
#define VEC3_H
#include "Maths/Vec2.hpp"
#include <math.h>

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
        inline Vec3 operator+(const Vec3 other) const 
        {
#ifdef USE_SSE
            float floats[4];

            __m128 A = _mm_set_ps(0.0f, Z, Y, X);
            __m128 B = _mm_set_ps(0.0f, other.Z, other.Y, other.X);
            A = _mm_add_ps(A, B);
            _mm_store_ps(floats, A);
            return *(Vec3 *)(floats);
#else
            return Vec3(X + other.X, Y + other.Y, Z + other.Z);
            #endif
        }
        inline void operator+=(const Vec3 other)
        {
            X += other.X;
            Y += other.Y;
            Z += other.Z;
        }
        inline Vec3 operator-(const Vec3 other) const 
        {
            return Vec3(X - other.X, Y - other.Y, Z - other.Z);
        }
        inline void operator-=(const Vec3 other)
        {
            X -= other.X;
            Y -= other.Y;
            Z -= other.Z;
        }
        inline Vec3 operator*(const Vec3 other) const
        {
            return Vec3(X * other.X, Y * other.Y, Z * other.Z);
        }
        inline void operator*=(const Vec3 other)
        {
            X *= other.X;
            Y *= other.Y;
            Z *= other.Z;
        }
        inline Vec3 operator/(const Vec3 other) const
        {
            return Vec3(X / other.X, Y / other.Y, Z / other.Z);
        }
        inline void operator/=(const Vec3 other)
        {
            X /= other.X;
            Y /= other.Y;
            Z /= other.Z;
        }
        inline Vec3 operator*(float amount) const
        {
            return Vec3(X * amount, Y * amount, Z * amount);
        }
        inline void operator*=(float amount)
        {
            X *= amount;
            Y *= amount;
            Z *= amount;
        }
        inline Vec3 operator/(float amount) const
        {
            float oneOver = 1.0f / amount;
            return Vec3(X * oneOver, Y * oneOver, Z * oneOver);
        }
        inline void operator/=(float amount)
        {
            float oneOver = 1.0f / amount;
            X *= oneOver;
            Y *= oneOver;
            Z *= oneOver;
        }
        inline Vec3 operator-() const
        {
            return Vec3(-X, -Y, -Z);
        }
        inline bool operator==(const Vec3 other) const
        {
            return X == other.X && Y == other.Y && Z == other.Z;
        }
        inline bool operator !=(const Vec3 other) const
        {
            return X != other.X || Y != other.Y || Z != other.Z;
        }
        inline float Length() const
        {
            return sqrtf(Dot(*this, *this));
        }
        inline float LengthSquared() const
        {
            return Dot(*this, *this);
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
            Vec3 result;
            float oneOverLength = 1.0f / sqrtf(X * X + Y * Y + Z * Z);
            result.X = X * oneOverLength;
            result.Y = Y * oneOverLength;
            result.Z = Z * oneOverLength;
            return result;
        }
        inline Vec2 ToVector2() const
        {
            return Maths::Vec2(X, Y);
        }
        static inline bool AlmostEqual(Vec3 A, Vec3 B, float epsilon = 0.01f)
        {
            return fabsf(B.X - A.X) <= epsilon && fabsf(B.Y - A.Y) <= epsilon && fabsf(B.Z - A.Z) <= epsilon;
        }
        static inline float Dot(const Vec3 A, const Vec3 B)
        {
            return A.X * B.X + A.Y * B.Y + A.Z * B.Z;
        }
        static inline Vec3 Max(const Vec3 A, const Vec3 B)
        {
            return Vec3(fmaxf(A.X, B.X), fmaxf(A.Y, B.Y), fmaxf(A.Z, B.Z));
        }
        static inline Vec3 Min(const Vec3 A, const Vec3 B)
        {
            return Vec3(fminf(A.X, B.X), fminf(A.Y, B.Y), fminf(A.Z, B.Z));
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
        static inline Vec3 Lerp(const Vec3 A, const Vec3 B, const float amount)
        {
            return Vec3(A.X + (B.X - A.X) * amount, A.Y + (B.Y - A.Y) * amount, A.Z + (B.Z - A.Z) * amount);
        }
        static inline Vec3 Cross(const Vec3 vec1, const Vec3 vec2)
        {
            return Vec3(
                vec1.Y * vec2.Z - vec1.Z * vec2.Y,
                vec1.Z * vec2.X - vec1.X * vec2.Z,
                vec1.X * vec2.Y - vec1.Y * vec2.X);
        }

        inline vec3 ToCVec() const
        {
            return {X, Y, Z};
        }
    };
}