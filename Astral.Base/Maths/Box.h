#pragma once
#include <stdint.h>
#include <math.h>
#include "./Vectors.h"

typedef struct
{
    float X;
    float Y;
    float width;
    float height;
} Box;

inline Box Box_From(float x, float y, float width, float height)
{
    const Box result = {x, y, width, height};
    return result;
}
inline Box Box_FromCorners(Vec2 pointA, Vec2 pointB)
{
    Vec2 minPoint = Vec2Min(pointA, pointB);
    Vec2 maxPoint = Vec2Max(pointA, pointB);
    const Box result = {minPoint.X, minPoint.Y, maxPoint.X - minPoint.X, maxPoint.Y - minPoint.Y};
    return result;
}
inline bool Box_Contains(Box self, Vec2 point)
{
    return point.X >= self.X && point.X <= (self.X + self.width) && point.Y >= self.Y && point.Y <= (self.Y + self.height);
}
#define Vec2toPoint2(point) CreatePoint2((float)point.X, (float)point.Y)
#define Rect2Box(box) Box_From((int32_t)box.X, (int32_t)box.Y, (int32_t)box.width, (int32_t)box.height)