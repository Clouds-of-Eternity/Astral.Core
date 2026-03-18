#pragma once
#include <math.h>
#include <stdint.h>

typedef struct Point2
{
    int32_t X;
    int32_t Y;
} Point2;

inline Point2 Point2_Empty()
{
    const Point2 result = {};
    return result;
}
inline Point2 CreatePoint2(int32_t x, int32_t y)
{
    Point2 result = {x, y};
    return result;
}

inline Point2 Point2Max(Point2 A, Point2 B)
{
    const Point2 result = {A.X > B.X ? A.X : B.X, A.Y > B.Y ? A.Y : B.Y};
    return result;
}
inline Point2 Point2Min(Point2 A, Point2 B)
{
    const Point2 result = {A.X < B.X ? A.X : B.X, A.Y < B.Y ? A.Y : B.Y};
    return result;
}