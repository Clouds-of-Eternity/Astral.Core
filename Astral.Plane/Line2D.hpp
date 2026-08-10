#pragma once
#include "Box.hpp"
#include "Maths/Vec2.hpp"

struct Line2D
{
    Maths::Vec2 start;
    Maths::Vec2 end;

    inline Line2D()
    {
        this->start = Maths::Vec2();
        this->end = Maths::Vec2();
    }
    inline Line2D(float x1, float y1, float x2, float y2)
    {
        this->start = Maths::Vec2(x1, y1);
        this->end = Maths::Vec2(x2, y2);
    }
    inline Line2D(Maths::Vec2 start, Maths::Vec2 end)
    {
        this->start = start;
        this->end = end;
    }
    inline Maths::Vec2 GetDir() const
    {
        return (end - start).Normalized();
    }
    inline Maths::Vec2 GetNormal(bool isClockwise) const
    {
        Maths::Vec2 lineDir = GetDir();
        float c;
        float s;
        if (isClockwise)
        {
            c = -1.0f;
            s = 0.0f;
        }
        else
        {
            c = 0.0f;
            s = 1.0f;
        }
        return Maths::Vec2(lineDir.X * c - lineDir.Y * s, lineDir.X * s + lineDir.Y * c);
    }
	inline bool Intersects(Line2D line2, Maths::Vec2 *hitPosition) const
	{
		Maths::Vec2 direction = (end - start).Normalized();

		float denominator = (start.X - end.X) * (line2.start.Y - line2.end.Y) - (start.Y - end.Y) * (line2.start.X - line2.end.X);
		float t = ((start.X - line2.start.X) * (line2.start.Y - line2.end.Y) - (start.Y - line2.start.Y) * (line2.start.X - line2.end.X)) / denominator;
		float u = ((start.X - line2.start.X) * (start.Y - end.Y) - (start.Y - line2.start.Y) * (start.X - end.X)) / denominator;

		if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
		{
			if (hitPosition == NULL)
			{
				return true;
			}
            *hitPosition = Maths::Vec2(line2.start.X + u * (line2.end.X - line2.start.X), line2.start.Y + u * (line2.end.Y - line2.start.Y));
            return true;
		}
		return false;
	}
    inline bool Intersects(Box box, Maths::Vec2 *hitPosition) const
    {
        if (box.Contains(start))
        {
            *hitPosition = start;
            return true;
        }

        //find the closest two edges
        const Maths::Vec2 dir = GetDir();
        const Maths::Vec2 vertices[4] = {
            box.GetTopLeft(),
            box.GetTopRight(),
            box.GetBottomRight(),
            box.GetBottomLeft()
        };
        float dots[4] = {};
        const float s = -1.0f;//sinf(-90d);
        const float c = 0.0f;

        i8 edge1 = -1;
        i8 edge2 = -1;
        for (u32 i = 0; i < 4; i++)
        {
            Maths::Vec2 normalDir = Line2D(vertices[i], vertices[(i + 1) % 2]).GetNormal(true);

            dots[i] = Maths::Vec2::Dot(dir, -normalDir);
        }
        //find the two last minimum dots products to find the two potential collider edges
        float minDot = 10.0f;
        for (u32 i = 0; i < 4; i++)
        {
            if (dots[i] < minDot)
            {
                edge2 = edge1;
                edge1 = (i8)i;
                minDot = dots[i];
            }
        }

        //find intersections
        Maths::Vec2 edge1IntersectPos = {};
        Maths::Vec2 edge2IntersectPos = {};
        Line2D line1 = Line2D(vertices[edge1], vertices[(edge1 + 1) % 4]);
        Line2D line2 = Line2D(vertices[edge2], vertices[(edge2 + 1) % 4]);

        bool edge1Intersects = Intersects(line1, &edge1IntersectPos);
        bool edge2Intersects = Intersects(line2, &edge2IntersectPos);

        if (edge1Intersects && edge2Intersects)
        {
            *hitPosition = Maths::Vec2::DistanceSquared(start, edge1IntersectPos) < Maths::Vec2::DistanceSquared(start, edge2IntersectPos) ? edge1IntersectPos : edge1IntersectPos;
            return true;
        }
        else if (edge1Intersects)
        {
            *hitPosition = edge1IntersectPos;
            return true;
        }
        else if (edge2Intersects)
        {
            *hitPosition = edge2IntersectPos;
            return true;
        }
        else
        {
            return false;
        }
    }
};