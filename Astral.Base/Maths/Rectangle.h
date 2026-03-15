#pragma once
#include <stdint.h>
#include <math.h>
#include "./Points.h"

typedef struct
{
    int32_t X;
    int32_t Y;
    int32_t width;
    int32_t height;
} Rectangle;

inline Rectangle Rectangle_From(int32_t x, int32_t y, int32_t width, int32_t height)
{
    const Rectangle result = {x, y, width, height};
    return result;
}
inline Rectangle Rectangle_FromCorners(Point2 pointA, Point2 pointB)
{
    Point2 minPoint = Point2Min(pointA, pointB);
    Point2 maxPoint = Point2Max(pointA, pointB);
    const Rectangle result = {minPoint.X, minPoint.Y, maxPoint.X - minPoint.X, maxPoint.Y - minPoint.Y};
    return result;
}
inline bool Rectangle_Contains(Rectangle self, Point2 point)
{
    return point.X >= self.X && point.X <= (self.X + self.width) && point.Y >= self.Y && point.Y <= (self.Y + self.height);
}
#define Point2Vec2(point) CreateVec2((float)point.X, (float)point.Y)
#define Box2Rect(box) Rectangle_From((int32_t)box.X, (int32_t)box.Y, (int32_t)box.width, (int32_t)box.height)