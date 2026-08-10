#pragma once
#include <math.h>
#include "Maths/CVectors.h"

namespace Maths
{
    struct Point2;
    
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
        inline Vec2 operator+(Vec2 other) const
        {
            return Vec2(X + other.X, Y + other.Y);
        }
        inline void operator+=(Vec2 other)
        {
            X += other.X;
            Y += other.Y;
        }

        inline Vec2 operator-(Vec2 other) const
        {
            return Vec2(X - other.X, Y - other.Y);
        }
        inline void operator-=(Vec2 other)
        {
            X -= other.X;
            Y -= other.Y;
        }

        inline Vec2 operator*(Vec2 other) const
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

        inline Vec2 operator/(Vec2 other) const
        {
            return Vec2(X / other.X, Y / other.Y);
        }
        inline void operator/=(Vec2 other)
        {
            X /= other.X;
            Y /= other.Y;
        }

        inline bool operator==(Vec2 other) const
        {
            return X == other.X && Y == other.Y;
        }
        inline bool operator !=(Vec2 other) const
        {
            return X != other.X || Y != other.Y;
        }
        inline bool operator >(Vec2 other) const
        {
            return X > other.X && Y > other.Y;
        }
        inline bool operator >=(Vec2 other) const
        {
            return X >= other.X && Y >= other.Y;
        }
        inline bool operator <(Vec2 other) const
        {
            return X < other.X && Y < other.Y;
        }
        inline bool operator <=(Vec2 other) const
        {
            return X <= other.X && Y <= other.Y;
        }
        inline Vec2 operator-() const
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
        static inline float DistanceSquared(Vec2 A, Vec2 B)
        {
            float dx = B.X - A.X;
            float dy = B.Y - A.Y;
            return dx * dx + dy * dy;
        }
        static inline Vec2 Lerp(Vec2 A, Vec2 B, float amount)
        {
            return Vec2(A.X + (B.X - A.X) * amount, A.Y + (B.Y - A.Y) * amount);
        }

        inline Point2 ToPoint2() const;
        inline vec2 ToCVec() const
        {
            return {X, Y};
        }
    };

    struct Point2
    {
        i32 X;
        i32 Y;

        inline Point2()
        {
            X = 0;
            Y = 0;
        }
        inline Point2(i32 x, i32 y)
        {
            X = x;
            Y = y;
        }
        inline Point2(i32 val)
        {
            X = val;
            Y = val;
        }
        inline bool operator==(Point2 other) const
        {
            return X == other.X && Y == other.Y;
        }
        inline bool operator !=(Point2 other) const
        {
            return X != other.X || Y != other.Y;
        }
        inline bool operator >(Point2 other) const
        {
            return X > other.X && Y > other.Y;
        }
        inline bool operator >=(Point2 other) const
        {
            return X >= other.X && Y >= other.Y;
        }
        inline bool operator <(Point2 other) const
        {
            return X < other.X && Y < other.Y;
        }
        inline bool operator <=(Point2 other) const
        {
            return X <= other.X && Y <= other.Y;
        }
        inline Point2 operator-() const
        {
            return Point2(-X, -Y);
        }

        inline Point2 operator+(Point2 B) const
        {
            return Point2(X + B.X, Y + B.Y);
        }
        inline void operator+=(Point2 B)
        {
            X += B.X;
            Y += B.Y;
        }
        inline Point2 operator-(Point2 B) const
        {
            return Point2(X - B.X, Y - B.Y);
        }
        inline void operator-=(Point2 B)
        {
            X -= B.X;
            Y -= B.Y;
        }
        inline Point2 operator*(Point2 B) const
        {
            return Point2(X * B.X, Y * B.Y);
        }
        inline void operator*=(Point2 B)
        {
            X *= B.X;
            Y *= B.Y;
        }
        inline Point2 operator/(Point2 B) const
        {
            return Point2(X / B.X, Y / B.Y);
        }
        inline void operator/=(Point2 B)
        {
            X /= B.X;
            Y /= B.Y;
        }

        static inline float Dot(Point2 A, Point2 B)
        {
            return A.X * B.X + A.Y * B.Y;
        }
        static inline Point2 Max(Point2 A, Point2 B)
        {
            return Point2(A.X > B.X ? A.X : B.X, A.Y > B.Y ? A.Y : B.Y);
        }
        static inline Point2 Min(Vec2 A, Vec2 B)
        {
            return Point2(A.X < B.X ? A.X : B.X, A.Y < B.Y ? A.Y : B.Y);
        }
        static inline float Distance(Point2 A, Point2 B)
        {
            float dx = B.X - A.X;
            float dy = B.Y - A.Y;
            return sqrtf(dx * dx + dy * dy);
        }
        static inline float DistanceSquared(Point2 A, Point2 B)
        {
            float dx = B.X - A.X;
            float dy = B.Y - A.Y;
            return dx * dx + dy * dy;
        }
        static inline Point2 Lerp(Point2 A, Point2 B, float amount)
        {
            return Point2(A.X + (B.X - A.X) * amount, A.Y + (B.Y - A.Y) * amount);
        }
        inline Vec2 ToVector2() const
        {
            return Vec2(X, Y);
        }
    };

    inline bool Point2Eql(Point2 A, Point2 B)
    {
        return A.X == B.X && A.Y == B.Y;
    }
    inline u32 Point2Hash(Point2 self)
    {
        return (u32)(self.X ^ (self.Y + 0x9e3779b9 + (self.X << 6) + (self.X >> 2)));
    }

    Point2 Vec2::ToPoint2() const
    {
        return Point2(X, Y);
    }
}