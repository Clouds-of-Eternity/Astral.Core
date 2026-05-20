#pragma once
#include "Maths/Point2.hpp"

namespace Maths
{
    struct Rectangle
    {
        i32 X;
        i32 Y;
        i32 Width;
        i32 Height;

        inline Rectangle()
        {
            X = 0;
            Y = 0;
            Width = 0;
            Height = 0;
        }
        inline Rectangle(i32 x, i32 y, i32 width, i32 height)
        {
            this->X = x;
            this->Y = y;
            this->Width = width;
            this->Height = height;
        }
        inline Rectangle(Maths::Point2 bounds1, Maths::Point2 bounds2)
        {
            this->X = bounds1.X < bounds2.X ? bounds1.X : bounds2.X;
            this->Y = bounds1.Y < bounds2.Y ? bounds1.Y : bounds2.Y;
            this->Width = bounds2.X - bounds1.X;
            this->Height = bounds2.Y - bounds1.Y;

            if (Width < 0)
            {
                Width *= -1;
            }
            if (Height < 0)
            {
                Height *= -1;
            }
        }

        inline i32 GetLeft() const
        {
            return this->X;
        }
        inline i32 GetRight() const
        {
            return (this->X + this->Width);
        }
        inline i32 GetTop() const
        {
            return this->Y;
        }
        inline i32 GetBottom() const
        {
            return (this->Y + this->Height);
        }
        inline Maths::Point2 GetTopLeft() const
        {
            return Maths::Point2(GetLeft(), GetTop());
        }
        inline Maths::Point2 GetTopRight() const
        {
            return Maths::Point2(GetRight(), GetTop());
        }
        inline Maths::Point2 GetBottomRight() const
        {
            return Maths::Point2(GetRight(), GetBottom());
        }
        inline Maths::Point2 GetBottomLeft() const
        {
            return Maths::Point2(GetLeft(), GetBottom());
        }
        inline Maths::Point2 GetPosition() const
        {
            return Maths::Point2(X, Y);
        }
        inline Maths::Point2 Center() const
        {
            return Maths::Point2(X + Width / 2, Y + Height / 2);
        }

        inline bool operator==(Rectangle other) const
        {
            return X == other.X && Y == other.Y && Width == other.Width && Height == other.Height;
        }
        inline bool operator!=(Rectangle other) const
        {
            return X != other.X || Y != other.Y || Width != other.Width || Height != other.Height;
        }

        inline bool Intersects(Rectangle other) const
        {
            return !(
                other.X > X + Width ||
                other.X + other.Width < X ||
                other.Y > Y + Height ||
                other.Y + other.Height < Y);
        }
        inline Rectangle IntersectionWith(Rectangle other) const
        {
            if (!Intersects(other))
            {
                return Rectangle();
            }
            i32 r = (X + Width) > (other.X + other.Width) ? X + Width : other.X + other.Width;
            i32 b = (Y + Height) > (other.Y + other.Height) ? Y + Height : other.Y + other.Height;
            i32 l = X < other.X ? X : other.X;
            i32 t = Y < other.Y ? Y : other.Y;
            return Rectangle(l, t, r - l, b - t);
        }
        inline bool Contains(Maths::Point2 point) const
        {
            return X <= point.X && point.X <= X + Width && Y <= point.Y && point.Y <= Y + Height;
        }
        inline bool Contains(Rectangle other) const
        {
            return Contains(other.GetTopLeft()) && Contains(other.GetBottomRight());
        }
    };
}
#define EMPTY_RECTANGLE Maths::Rectangle(0, 0, 0, 0)