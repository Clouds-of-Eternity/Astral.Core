#pragma once
#define VEC4_H
#include "Maths/Vec3.hpp"

namespace Maths
{
    struct Vec4
    {
        union
        {
            struct
            {
                float X, Y, Z, W;
            };
#ifdef USE_SSE
            __m128 asM128;
#endif
        };

        #ifdef USE_SSE
        #include "Maths/Vec4Impl_SSE.inc"
        #else
        #include "Maths/Vec4Impl_Default.inc"
        #endif

        inline float Length() const
        {
            return sqrtf(Dot(*this, *this));
        }
        static inline bool AlmostEqual(Vec4 A, Vec4 B, float epsilon = 0.01f)
        {
            Vec4 diff = Abs(B - A);
            return diff <= Vec4(epsilon);
        }
        inline Maths::Vec3 ToVector3() const
        {
            return Maths::Vec3(X, Y, Z);
        }
        inline Maths::Vec2 ToVector2() const
        {
            return Maths::Vec2(X, Y);
        }

        inline vec4 ToCVec() const
        {
            return {X, Y, Z};
        }
    };
}