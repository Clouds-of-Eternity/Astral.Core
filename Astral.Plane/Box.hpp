#pragma once
#include "Maths/Vec2.hpp"
#include "Maths/Util.hpp"
#include "Circle.hpp"

struct Rect32;

struct Box
{
    float X;
    float Y;
    float width;
    float height;

    inline Box()
    {
        X = 0.0f;
        Y = 0.0f;
        width = 0.0f;
        height = 0.0f;
    }
    inline Box(float X, float Y, float width, float height)
    {
        this->X = X;
        this->Y = Y;
        this->width = width;
        this->height = height;
    }
    inline Box(Maths::Vec2 pos, float width, float height)
    {
        if (width < 0.0f)
        {
            pos.X += width;
            width = -width;
        }
        if (height < 0.0f)
        {
            pos.Y += height;
            height = -height;
        }
        this->X = pos.X;
        this->Y = pos.Y;
        this->width = width;
        this->height = height;
    }
    inline Box(Maths::Vec2 topLeft, Maths::Vec2 bottomRight)
    {
        float minX = fminf(topLeft.X, bottomRight.X);
        float minY = fminf(topLeft.Y, bottomRight.Y);

        float maxX = fmaxf(topLeft.X, bottomRight.X);
        float maxY = fmaxf(topLeft.Y, bottomRight.Y);

        X = minX;
        Y = minY;

        this->width = maxX - minX;
        this->height = maxY - minY;
    }

    // CONTAIN METHODS
    inline bool Contains(Maths::Vec2 point) const
    {
        return X <= point.X && point.X <= X + width && Y <= point.Y && point.Y <= Y + height;
    }
    inline bool Contains(Box other) const
    {
        return Contains(other.GetTopLeft()) && Contains(other.GetBottomRight());
    }

    // COORDINATE PROPERTIES
    inline float GetLeft() const
    {
        return this->X;
    }
    inline float GetRight() const
    {
        return (this->X + this->width);
    }
    inline float GetTop() const
    {
        return this->Y;
    }
    inline float GetBottom() const
    {
        return (this->Y + this->height);
    }
    inline Maths::Vec2 GetTopLeft() const
    {
        return Maths::Vec2(GetLeft(), GetTop());
    }
    inline Maths::Vec2 GetTopRight() const
    {
        return Maths::Vec2(GetRight(), GetTop());
    }
    inline Maths::Vec2 GetBottomRight() const
    {
        return Maths::Vec2(GetRight(), GetBottom());
    }
    inline Maths::Vec2 GetBottomLeft() const
    {
        return Maths::Vec2(GetLeft(), GetBottom());
    }
    inline Maths::Vec2 GetPosition() const
    {
        return Maths::Vec2(X, Y);
    }
    inline Maths::Vec2 Center() const
    {
        return Maths::Vec2(X + width * 0.5f, Y + height * 0.5f);
    }

    // CONVERSIONS
    inline void GetAsVerticesRotated(Maths::Vec2 *array, float sin, float cos, Maths::Vec2 pivot)
    {
        Maths::Vec2 scale = Maths::Vec2(1.0f);
        array[0] = Maths::RotateAbout(GetTopLeft() * scale, pivot, sin, cos);
        array[1] = Maths::RotateAbout(GetBottomLeft() * scale, pivot, sin, cos);
        array[2] = Maths::RotateAbout(GetBottomRight() * scale, pivot, sin, cos);
        array[3] = Maths::RotateAbout(GetTopRight() * scale, pivot, sin, cos);
    }
    inline void GetAsVerticesRotated(Maths::Vec2 *array, float sin, float cos, Maths::Vec2 pivot, Maths::Vec2 scale)
    {
        array[0] = Maths::RotateAbout(GetTopLeft() * scale, pivot, sin, cos);
        array[1] = Maths::RotateAbout(GetBottomLeft() * scale, pivot, sin, cos);
        array[2] = Maths::RotateAbout(GetBottomRight() * scale, pivot, sin, cos);
        array[3] = Maths::RotateAbout(GetTopRight() * scale, pivot, sin, cos);
    }

    // OPERATIONS
    inline Box Inflate(float x, float y) const
    {
        return Box(X - x, Y - y, width + x * 2.0f, height + y * 2.0f);
    }
    inline static Box Union(Box value1, Box value2)
    {
        if (value1 == Box())
        {
            return value2;
        }
        float x = AC_MIN(value1.X, value2.X);
        float y = AC_MIN(value1.Y, value2.Y);
        return Box(
            x,
            y,
            AC_MAX(value1.X + value1.width, value2.X + value2.width) - x,  // max of rights  - x
            AC_MAX(value1.Y + value1.height, value2.Y + value2.height) - y // max of bottoms - y
        );
    }
    inline Box Union(Box value2) const
    {
        return Union(*this, value2);
    }
    inline float SurfaceArea() const
    {
        return width * height;
    }
    inline Maths::Vec2 FurthestPoint(Maths::Vec2 direction) const
    {
        Maths::Vec2 maxPoint = Maths::Vec2();
        float maxDistance = AC_FloatMin;

        {
            float distance = Maths::Vec2::Dot(GetTopLeft(), direction);
            if (distance > maxDistance)
            {
                maxDistance = distance;
                maxPoint = GetTopLeft();
            }
        }
        {
            float distance = Maths::Vec2::Dot(GetTopRight(), direction);
            if (distance > maxDistance)
            {
                maxDistance = distance;
                maxPoint = GetTopRight();
            }
        }
        {
            float distance = Maths::Vec2::Dot(GetBottomRight(), direction);
            if (distance > maxDistance)
            {
                maxDistance = distance;
                maxPoint = GetBottomRight();
            }
        }
        {
            float distance = Maths::Vec2::Dot(GetBottomLeft(), direction);
            if (distance > maxDistance)
            {
                maxPoint = GetBottomLeft();
            }
        }
        return maxPoint;
    }

    // INTERSECTIONS
    inline bool Intersects(Box other, float allowedApproximation = 0.0f) const
    {
        // return !(
        //     other.X > X + width ||
        //     other.X + other.width < X ||
        //     other.Y > Y + height ||
        //     other.Y + other.height < Y);

        return 
            other.X <= X + width + allowedApproximation &&
            other.X + other.width >= X - allowedApproximation &&
            other.Y <= Y + height + allowedApproximation &&
            other.Y + other.height >= Y - allowedApproximation;
    }
    /// @brief Tests if this box and the other box have edges that touch each other but ultimately
    /// do not overlap.
    inline i8 Touches(Box other, float allowedApproximation = 0.0f) const
    {
        const float otherLeft = other.X;
        const float otherTop = other.Y;
        const float otherRight = other.GetRight();
        const float otherBtm = other.GetBottom();

        const float left = X;
        const float top = Y;
        const float right = GetRight();
        const float btm = GetBottom();

        //left -> other right
        if (fabsf(left - otherRight) <= allowedApproximation)
        {
            return 0;
        }
        //top -> other bottom
        else if (fabsf(otherBtm - top) <= allowedApproximation)
        {
            return 1;
        }
        //right -> other left
        else if (fabsf(otherLeft - right) <= allowedApproximation)
        {
            return 2;
        }
        //bottom -> other top
        else if (fabsf(otherTop - btm) <= allowedApproximation)
        {
            return 3;
        }
        else return -1;
    }
    inline Box IntersectionWith(Box other) const
    {
        Box result = *this;
        if (result.Y < other.Y)
        {
            result.height -= other.Y - result.Y;
            result.Y = other.Y;
        }
        if (result.Y + result.height > other.Y + other.height)
        {
            result.height += other.Y + other.height - result.Y - result.height;
        }
        if (result.X < other.X)
        {
            result.width -= other.X - result.X;
            result.X = other.X;
        }
        if (result.X + result.width > other.X + other.width)
        {
            result.width += other.X + other.width - result.X - result.width;
        }
        
        if (result.height <= 0.0f || result.width <= 0.0f)
        {
            return Box();
        }
        return result;
    }
    inline bool CheckIntersectionWith(Box other, Box *output) const
    {
        bool intersected = false;
        Box result = *this;
        if (result.Y < other.Y)
        {
            result.height -= other.Y - result.Y;
            result.Y = other.Y;
            intersected = true;
        }
        if (result.Y + result.height > other.Y + other.height)
        {
            result.height += other.Y + other.height - result.Y - result.height;
            intersected = true;
        }
        if (result.X < other.X)
        {
            result.width -= other.X - result.X;
            result.X = other.X;
            intersected = true;
        }
        if (result.X + result.width > other.X + other.width)
        {
            result.width += other.X + other.width - result.X - result.width;
            intersected = true;
        }
        
        if (result.height <= 0.0f || result.width <= 0.0f)
        {
            *output = Box();
        }
        else *output = result;

        return intersected;
    }
    inline bool Intersects(Circle circle) const
    {
        if (Contains(circle.center))
        {
            return true;
        }
        float centerX = X + width * 0.5f;
        float centerY = Y + height * 0.5f;

        float distX = fabsf(circle.center.X - centerX);
        float distY = fabsf(circle.center.Y - centerY);

        if (distX > width * 0.5f + circle.radius || distY > height * 0.5f + circle.radius)
            return false;

        float cornerDistSquared = (distX - width * 0.5f) * (distX - width * 0.5f) + (distY - height * 0.5f) * (distY - height * 0.5f);
        return cornerDistSquared <= (circle.radius * circle.radius);
    }

    // OPERATOR OVERLOADS
    inline Box operator+(Maths::Vec2 offset) const
    {
        return Box(X + offset.X, Y + offset.Y, width, height);
    }
    inline Box operator+=(Maths::Vec2 offset)
    {
        X += offset.X;
        Y += offset.Y;
        return *this;
    }

    inline Box operator-(Maths::Vec2 offset) const
    {
        return Box(X - offset.X, Y - offset.Y, width, height);
    }
    inline Box operator-=(Maths::Vec2 offset)
    {
        X -= offset.X;
        Y -= offset.Y;
        return *this;
    }

    inline bool operator==(Box other) const
    {
        return X == other.X && Y == other.Y && width == other.width && height == other.height;
    }
    inline bool operator!=(Box other) const
    {
        return X != other.X || Y != other.Y || width != other.width || height != other.height;
    }

    inline Box operator*(Maths::Vec2 vec) const
    {
        Box result = Box(this->X * vec.X, this->Y * vec.Y, this->width * vec.X, this->height * vec.Y);
        if (result.width < 0.0f)
        {
            result.X += result.width;
            result.width *= -1.0f;
        }
        if (result.height < 0.0f)
        {
            result.Y += result.height;
            result.height *= -1.0f;
        }
        return result;
    }
    inline Box operator*=(Maths::Vec2 vec)
    {
        *this = *this * vec;
        return *this;
    }

    inline Rect32 ToRectangle() const;
};

struct Rect32
{
    i32 X;
    i32 Y;
    i32 width;
    i32 height;

    inline Rect32()
    {
        X = 0.0f;
        Y = 0.0f;
        width = 0.0f;
        height = 0.0f;
    }
    inline Rect32(float X, float Y, float width, float height)
    {
        this->X = (i32)X;
        this->Y = (i32)Y;
        this->width = (i32)width;
        this->height = (i32)height;
    }
    inline Rect32(i32 X, i32 Y, i32 width, i32 height)
    {
        this->X = X;
        this->Y = Y;
        this->width = width;
        this->height = height;
    }
    inline Rect32(Maths::Point2 pos, i32 width, i32 height)
    {
        if (width < 0)
        {
            pos.X += width;
            width = -width;
        }
        if (height < 0)
        {
            pos.Y += height;
            height = -height;
        }
        this->X = pos.X;
        this->Y = pos.Y;
        this->width = width;
        this->height = height;
    }
    inline Rect32(Maths::Point2 topLeft, Maths::Point2 bottomRight)
    {
        i32 minX = AC_MIN(topLeft.X, bottomRight.X);
        i32 minY = AC_MIN(topLeft.Y, bottomRight.Y);

        i32 maxX = AC_MIN(topLeft.X, bottomRight.X);
        i32 maxY = AC_MIN(topLeft.Y, bottomRight.Y);

        X = minX;
        Y = minY;

        this->width = maxX - minX;
        this->height = maxY - minY;
    }

    // CONTAIN METHODS
    inline bool Contains(Maths::Vec2 point) const
    {
        return X <= point.X && point.X <= X + width && Y <= point.Y && point.Y <= Y + height;
    }
    inline bool Contains(Maths::Point2 point) const
    {
        return X <= point.X && point.X <= X + width && Y <= point.Y && point.Y <= Y + height;
    }
    inline bool Contains(Rect32 other) const
    {
        return Contains(other.GetTopLeft()) && Contains(other.GetBottomRight());
    }

    // COORDINATE PROPERTIES
    inline i32 GetLeft() const
    {
        return this->X;
    }
    inline i32 GetRight() const
    {
        return (this->X + this->width);
    }
    inline i32 GetTop() const
    {
        return this->Y;
    }
    inline i32 GetBottom() const
    {
        return (this->Y + this->height);
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
        return Maths::Point2(X + width / 2, Y + height / 2);
    }

    // OPERATIONS
    inline Rect32 Inflate(i32 x, i32 y) const
    {
        return Rect32(X - x, Y - y, width + x * 2, height + y * 2);
    }
    inline static Rect32 Union(Rect32 value1, Rect32 value2)
    {
        if (value1 == Rect32())
        {
            return value2;
        }
        float x = AC_MIN(value1.X, value2.X);
        float y = AC_MIN(value1.Y, value2.Y);
        return Rect32(
            x,
            y,
            AC_MAX(value1.X + value1.width, value2.X + value2.width) - x,  // max of rights  - x
            AC_MAX(value1.Y + value1.height, value2.Y + value2.height) - y // max of bottoms - y
        );
    }
    inline Rect32 Union(Rect32 value2) const
    {
        return Union(*this, value2);
    }
    inline Box ToBox() const
    {
        return Box(X, Y, width, height);
    }
    inline i32 SurfaceArea() const
    {
        return width * height;
    }
    inline Maths::Vec2 FurthestPoint(Maths::Vec2 direction) const
    {
        return ToBox().FurthestPoint(direction);
    }

    // INTERSECTIONS
    inline bool Intersects(Rect32 other) const
    {
        return !(
            other.X > X + width ||
            other.X + other.width < X ||
            other.Y > Y + height ||
            other.Y + other.height < Y);
    }
    inline Rect32 IntersectionWith(Rect32 other) const
    {
        Rect32 result = *this;
        if (result.Y < other.Y)
        {
            result.height -= other.Y - result.Y;
            result.Y = other.Y;
        }
        if (result.Y + result.height > other.Y + other.height)
        {
            result.height += other.Y + other.height - result.Y - result.height;
        }
        if (result.X < other.X)
        {
            result.width -= other.X - result.X;
            result.X = other.X;
        }
        if (result.X + result.width > other.X + other.width)
        {
            result.width += other.X + other.width - result.X - result.width;
        }
        
        if (result.height <= 0 || result.width <= 0)
        {
            return Rect32();
        }
        return result;
    }
    inline bool CheckIntersectionWith(Rect32 other, Rect32 *output) const
    {
        bool intersected = false;
        Rect32 result = *this;
        if (result.Y < other.Y)
        {
            result.height -= other.Y - result.Y;
            result.Y = other.Y;
            intersected = true;
        }
        if (result.Y + result.height > other.Y + other.height)
        {
            result.height += other.Y + other.height - result.Y - result.height;
            intersected = true;
        }
        if (result.X < other.X)
        {
            result.width -= other.X - result.X;
            result.X = other.X;
            intersected = true;
        }
        if (result.X + result.width > other.X + other.width)
        {
            result.width += other.X + other.width - result.X - result.width;
            intersected = true;
        }
        
        if (result.height <= 0 || result.width <= 0)
        {
            *output = Rect32();
        }
        else *output = result;

        return intersected;
    }
    inline bool Intersects(Circle circle) const
    {
        return ToBox().Intersects(circle);
    }

    // OPERATOR OVERLOADS
    inline Rect32 operator+(Maths::Point2 offset) const
    {
        return Rect32(X + offset.X, Y + offset.Y, width, height);
    }
    inline Rect32 operator+=(Maths::Point2 offset)
    {
        X += offset.X;
        Y += offset.Y;
        return *this;
    }

    inline Rect32 operator-(Maths::Point2 offset) const
    {
        return Rect32(X - offset.X, Y - offset.Y, width, height);
    }
    inline Rect32 operator-=(Maths::Point2 offset)
    {
        X -= offset.X;
        Y -= offset.Y;
        return *this;
    }

    inline bool operator==(Rect32 other) const
    {
        return X == other.X && Y == other.Y && width == other.width && height == other.height;
    }
    inline bool operator!=(Rect32 other) const
    {
        return X != other.X || Y != other.Y || width != other.width || height != other.height;
    }

    inline Rect32 operator*(Maths::Point2 vec) const
    {
        Rect32 result = Rect32(this->X * vec.X, this->Y * vec.Y, this->width * vec.X, this->height * vec.Y);
        if (result.width < 0)
        {
            result.X += result.width;
            result.width *= -1;
        }
        if (result.height < 0)
        {
            result.Y += result.height;
            result.height *= -1;
        }
        return result;
    }
    inline Rect32 operator*=(Maths::Point2 vec)
    {
        *this = *this * vec;
        return *this;
    }
};

Rect32 Box::ToRectangle() const
{
    return Rect32((i32)X, (i32)Y, (i32)width, (i32)height);
}