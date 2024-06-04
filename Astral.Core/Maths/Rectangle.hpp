#pragma once
#include "Linxc.h"

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

        inline bool operator==(Rectangle other)
        {
            return X == other.X && Y == other.Y && Width == other.Width && Height == other.Height;
        }
        inline bool operator!=(Rectangle other)
        {
            return X != other.X || Y != other.Y || Width != other.Width || Height != other.Height;
        }
    };
}
#define EMPTY_RECTANGLE Maths::Rectangle(0,0,0,0)