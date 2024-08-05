#pragma once
#define VEC2_H
#include <math.h>

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
        inline Vec2 operator+(Vec2 other)
        {
            return Vec2(X + other.X, Y + other.Y);
        }
        inline void operator+=(Vec2 other)
        {
            X += other.X;
            Y += other.Y;
        }

        inline Vec2 operator-(Vec2 other)
        {
            return Vec2(X - other.X, Y - other.Y);
        }
        inline void operator-=(Vec2 other)
        {
            X -= other.X;
            Y -= other.Y;
        }

        inline Vec2 operator*(Vec2 other)
        {
            return Vec2(X * other.X, Y * other.Y);
        }
        inline void operator*=(Vec2 other)
        {
            X *= other.X;
            Y *= other.Y;
        }
        inline Vec2 operator*(float value)
        {
            return Vec2(X * value, Y * value);
        }

        inline Vec2 operator/(Vec2 other)
        {
            return Vec2(X / other.X, Y / other.Y);
        }
        inline void operator/=(Vec2 other)
        {
            X /= other.X;
            Y /= other.Y;
        }

        inline bool operator==(Vec2 other)
        {
            return X == other.X && Y == other.Y;
        }
        inline bool operator !=(Vec2 other)
        {
            return X != other.X || Y != other.Y;
        }
        inline Vec2 operator-()
        {
            return Vec2(-X, -Y);
        }
        inline float Length()
        {
            return sqrtf(X * X + Y * Y);
        }
        inline float LengthSquared()
        {
            return X * X + Y * Y;
        }
        inline void Normalize()
        {
            float oneOverLength = 1.0f / sqrtf(X * X + Y * Y);
            X *= oneOverLength;
            Y *= oneOverLength;
        }
        inline Vec2 Normalized()
        {
            Vec2 result;
            float oneOverLength = 1.0f / sqrtf(X * X + Y * Y);
            result.X = X * oneOverLength;
            result.Y = Y * oneOverLength;
            return result;
        }
        static inline float Dot(Vec2 A, Vec2 B)
        {
            return A.X * B.X + A.Y * B.Y;
        }
        static inline Vec2 Max(Vec2 A, Vec2 B)
        {
            return Vec2(A.X > B.X ? A.X : B.X, A.Y > B.Y ? A.Y : B.Y);
        }
        static inline Vec2 Min(Vec2 A, Vec2 B)
        {
            return Vec2(A.X < B.X ? A.X : B.X, A.Y < B.Y ? A.Y : B.Y);
        }
        static inline float Distance(Vec2 A, Vec2 B)
        {
            float dx = B.X - A.X;
            float dy = B.Y - A.Y;
            return sqrtf(dx * dx + dy * dy);
        }
        static inline float DistanceSquare(Vec2 A, Vec2 B)
        {
            float dx = B.X - A.X;
            float dy = B.Y - A.Y;
            return dx * dx + dy * dy;
        }
        static inline Vec2 Lerp(Vec2 A, Vec2 B, float amount)
        {
            return Vec2(A.X + (B.X - A.X) * amount, A.Y + (B.Y - A.Y) * amount);
        }
    };
}