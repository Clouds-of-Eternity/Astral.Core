#pragma once
#define VEC2_H
#include <math.h>
#include "Maths/CVectors.h"

namespace Maths
{
    struct Vec2
    {
        float X;
        float Y;

        inline Vec2()
        {
            X = 0.0f;
            Y = 0.0f;
        };
        inline Vec2(float x, float y)
        {
            X = x;
            Y = y;
        }
        inline Vec2(float val)
        {
            X = val;
            Y = val;
        }
        inline Vec2(vec2 cvec)
        {
            X = cvec.X;
            Y = cvec.Y;
        }
        inline Vec2 operator+(const Vec2 other) const
        {
            return Vec2(X + other.X, Y + other.Y);
        }
        inline void operator+=(const Vec2 other)
        {
            X += other.X;
            Y += other.Y;
        }

        inline Vec2 operator-(const Vec2 other) const
        {
            return Vec2(X - other.X, Y - other.Y);
        }
        inline void operator-=(const Vec2 other)
        {
            X -= other.X;
            Y -= other.Y;
        }

        inline Vec2 operator*(const Vec2 other) const
        {
            return Vec2(X * other.X, Y * other.Y);
        }
        inline void operator*=(const Vec2 other)
        {
            X *= other.X;
            Y *= other.Y;
        }
        inline Vec2 operator*(const float value)
        {
            return Vec2(X * value, Y * value);
        }

        inline Vec2 operator/(const Vec2 other) const
        {
            return Vec2(X / other.X, Y / other.Y);
        }
        inline void operator/=(const Vec2 other)
        {
            X /= other.X;
            Y /= other.Y;
        }

        inline bool operator==(const Vec2 other) const
        {
            return X == other.X && Y == other.Y;
        }
        inline bool operator !=(const Vec2 other) const
        {
            return X != other.X || Y != other.Y;
        }
        inline Vec2 operator-()
        {
            return Vec2(-X, -Y);
        }
        inline float Length() const
        {
            return sqrtf(X * X + Y * Y);
        }
        inline float LengthSquared() const
        {
            return X * X + Y * Y;
        }
        inline void Normalize()
        {
            float oneOverLength = 1.0f / sqrtf(X * X + Y * Y);
            X *= oneOverLength;
            Y *= oneOverLength;
        }
        inline Vec2 Normalized() const
        {
            Vec2 result;
            float oneOverLength = 1.0f / sqrtf(X * X + Y * Y);
            result.X = X * oneOverLength;
            result.Y = Y * oneOverLength;
            return result;
        }
        static inline bool AlmostEqual(Vec2 A, Vec2 B, float epsilon = 0.01f)
        {
            return fabsf(B.X - A.X) <= epsilon && fabsf(B.Y - A.Y) <= epsilon;
        }
        static inline float Dot(const Vec2 A, const Vec2 B)
        {
            return A.X * B.X + A.Y * B.Y;
        }
        static inline Vec2 Max(const Vec2 A, const Vec2 B)
        {
            return Vec2(A.X > B.X ? A.X : B.X, A.Y > B.Y ? A.Y : B.Y);
        }
        static inline Vec2 Min(const Vec2 A, const Vec2 B)
        {
            return Vec2(A.X < B.X ? A.X : B.X, A.Y < B.Y ? A.Y : B.Y);
        }
        static inline float Distance(const Vec2 A, const Vec2 B)
        {
            float dx = B.X - A.X;
            float dy = B.Y - A.Y;
            return sqrtf(dx * dx + dy * dy);
        }
        static inline float DistanceSquared(const Vec2 A, const Vec2 B)
        {
            float dx = B.X - A.X;
            float dy = B.Y - A.Y;
            return dx * dx + dy * dy;
        }
        static inline Vec2 Lerp(const Vec2 A, const Vec2 B, const float amount)
        {
            return Vec2(A.X + (B.X - A.X) * amount, A.Y + (B.Y - A.Y) * amount);
        }

        inline vec2 ToCVec() const
        {
            return {X, Y};
        }
    };
}