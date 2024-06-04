#pragma once
#include "Linxc.h"

namespace Maths
{
    struct Point2
    {
        i32 X;
        i32 Y;

        Point2()
        {
            X = 0;
            Y = 0;
        }
        Point2(i32 x, i32 y)
        {
            X = x;
            Y = y;
        }
        Point2(i32 val)
        {
            X = val;
            Y = val;
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
            return Vec2((float)(-X), (float)(-Y));
        }
    };
}