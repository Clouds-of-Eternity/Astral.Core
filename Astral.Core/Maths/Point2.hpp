#pragma once
#define POINT2
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
        inline bool operator==(Point2 other)
        {
            return X == other.X && Y == other.Y;
        }
        inline bool operator !=(Point2 other)
        {
            return X != other.X || Y != other.Y;
        }
        inline Point2 operator-()
        {
            return Point2(-X, -Y);
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
}