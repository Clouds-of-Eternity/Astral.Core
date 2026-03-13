#pragma once
#include <math.h>

#ifndef FORCE_NO_INTRINSICS
#ifdef __x86_64__

#define USE_SSE
#include <immintrin.h>

#endif
#endif

typedef struct
{
    float X;
    float Y;
} Vec2;

typedef struct
{
    float X;
    float Y;
    float Z;
} Vec3;

typedef struct
{
    union
    {
        struct
        {
            float X;
            float Y;
            float Z;
            float W;
        };
#ifdef USE_SSE
        __m128 m128;
#endif
    };
} Vec4;

inline Vec2 CreateVec2(float X, float Y)
{
    const Vec2 result = {X, Y};
    return result;
}
inline Vec3 CreateVec3(float X, float Y, float Z)
{
    const Vec3 result = {X, Y, Z};
    return result;
}
inline Vec4 CreateVec4(float X, float Y, float Z, float W)
{
    #ifdef USE_SSE
    const Vec4 result = {.m128 = _mm_setr_ps(X, Y, Z, W)};
    #else
    const Vec4 result = {X, Y, Z, W};
    #endif
    return result;
}
inline Vec4 CreateVec4Ptr(const float *components)
{
    #ifdef USE_SSE
    const Vec4 result = {.m128 = _mm_load_ps(components)};
    #else
    const Vec4 result = {components[0], components[1], components[2], components[3]};
    #endif
    return result;
}
inline Vec4 CreateVec4Scalar(float value)
{
    #ifdef USE_SSE
    const Vec4 result = {.m128 = _mm_set1_ps(value)};
    #else
    const Vec4 result = {value, value, value, value};
    #endif
    return result;
}

inline Vec3 Vec2to3(Vec2 vec, float Z)
{
    const Vec3 result = {vec.X, vec.Y, Z};
    return result;
}
inline Vec4 Vec2to4(Vec2 vec, float Z, float W)
{
    const Vec4 result = {vec.X, vec.Y, Z, W};
    return result;
}

inline Vec2 Vec3to2(Vec3 vec)
{
    const Vec2 result = {vec.X, vec.Y};
    return result;
}
inline Vec4 Vec3to4(Vec3 vec, float W)
{
    const Vec4 result = {vec.X, vec.Y, vec.Z, W};
    return result;
}

inline Vec2 Vec4to2(Vec4 vec)
{
    const Vec2 result = {vec.X, vec.Y};
    return result;
}
inline Vec3 Vec4to3(Vec4 vec)
{
    const Vec3 result = {vec.X, vec.Y, vec.Z};
    return result;
}

inline Vec2 AddVec2(Vec2 A, Vec2 B)
{
    const Vec2 result = {A.X + B.X, A.Y + B.Y};
    return result;
}
inline Vec3 AddVec3(Vec3 A, Vec3 B)
{
    const Vec3 result = {A.X + B.X, A.Y + B.Y, A.Z + B.Z};
    return result;
}

inline Vec2 AddVec2(Vec2 A, Vec2 B)
{
    const Vec2 result = {A.X + B.X, A.Y + B.Y};
    return result;
}
inline Vec2 SubVec2(Vec2 A, Vec2 B)
{
    const Vec2 result = {A.X - B.X, A.Y - B.Y};
    return result;
}
inline Vec2 MulVec2(Vec2 A, Vec2 B)
{
    const Vec2 result = {A.X * B.X, A.Y * B.Y};
    return result;
}
inline Vec2 MulVec2Flt(Vec2 A, float B)
{
    const Vec2 result = {A.X * B, A.Y * B};
    return result;
}
inline Vec2 DivVec2(Vec2 A, Vec2 B)
{
    const Vec2 result = {A.X / B.X, A.Y / B.Y};
    return result;
}
inline Vec2 DivVec2Flt(Vec2 A, float B)
{
    const Vec2 result = {A.X / B, A.Y / B};
    return result;
}
inline float Vec2Dot(Vec2 A, Vec2 B)
{
    return A.X * B.X + A.Y * B.Y;
}
inline float Vec2Length(Vec2 A)
{
    return sqrtf(Vec2Dot(A, A));
}
inline Vec2 Vec2Max(Vec2 A, Vec2 B)
{
    const Vec2 result = {fmaxf(A.X, B.X), fmaxf(A.Y, B.Y)};
    return result;
}
inline Vec2 Vec2Min(Vec2 A, Vec2 B)
{
    const Vec2 result = {fminf(A.X, B.X), fminf(A.Y, B.Y)};
    return result;
}

inline Vec3 AddVec3(Vec3 A, Vec3 B)
{
    const Vec3 result = {A.X + B.X, A.Y + B.Y, A.Z + B.Z};
    return result;
}
inline Vec3 SubVec3(Vec3 A, Vec3 B)
{
    const Vec3 result = {A.X - B.X, A.Y - B.Y, A.Z - B.Z};
    return result;
}
inline Vec3 MulVec3(Vec3 A, Vec3 B)
{
    const Vec3 result = {A.X * B.X, A.Y * B.Y, A.Z * B.Z};
    return result;
}
inline Vec3 MulVec3Flt(Vec3 A, float B)
{
    const Vec3 result = {A.X * B, A.Y * B, A.Z * B};
    return result;
}
inline Vec3 DivVec3(Vec3 A, Vec3 B)
{
    const Vec3 result = {A.X / B.X, A.Y / B.Y, A.Z / B.Z};
    return result;
}
inline Vec3 DivVec3Flt(Vec3 A, float B)
{
    const Vec3 result = {A.X / B, A.Y / B, A.Z / B};
    return result;
}
inline float Vec3Dot(Vec3 A, Vec3 B)
{
    return A.X * B.X + A.Y * B.Y + A.Z * B.Z;
}
inline float Vec3Length(Vec3 A)
{
    return sqrtf(Vec3Dot(A, A));
}
inline Vec3 Vec3Max(Vec3 A, Vec3 B)
{
    const Vec3 result = {fmaxf(A.X, B.X), fmaxf(A.Y, B.Y), fmaxf(A.Z, B.Z)};
    return result;
}
inline Vec3 Vec3Min(Vec3 A, Vec3 B)
{
    const Vec3 result = {fminf(A.X, B.X), fminf(A.Y, B.Y), fminf(A.Z, B.Z)};
    return result;
}

inline Vec4 AddVec4(Vec4 A, Vec4 B)
{
#ifdef USE_SSE
    const Vec4 result = {.m128 = _mm_add_ps(A.m128, B.m128)};
#else
    const Vec2 result = {A.X + B.X, A.Y + B.Y, A.Z + B.Z, A.W + B.W};
#endif
    return result;
}
inline Vec4 SubVec4(Vec4 A, Vec4 B)
{
#ifdef USE_SSE
    const Vec4 result = {.m128 = _mm_sub_ps(A.m128, B.m128)};
#else
    const Vec4 result = {A.X - B.X, A.Y - B.Y, A.Z - B.Z, A.W - B.W};
#endif
    return result;
}
inline Vec4 MulVec4(Vec4 A, Vec4 B)
{
#ifdef USE_SSE
    const Vec4 result = {.m128 = _mm_mul_ps(A.m128, B.m128)};
#else
    const Vec4 result = {A.X * B.X, A.Y * B.Y, A.Z * B.Z, A.W * B.W};
#endif
    return result;
}
inline Vec4 MulVec4Flt(Vec4 A, float B)
{
#ifdef USE_SSE
    const __m128 BVec4 = _mm_set1_ps(B);
    const Vec4 result = {.m128 = _mm_mul_ps(A.m128, BVec4)};
#else
    const Vec4 result = {A.X * B, A.Y * B, A.Z * B, A.W * B};
#endif
    return result;
}
inline Vec4 DivVec4(Vec4 A, Vec4 B)
{
#ifdef USE_SSE
    const Vec4 result = {.m128 = _mm_div_ps(A.m128, B.m128)};
#else
    const Vec4 result = {A.X / B.X, A.Y / B.Y, A.Z / B.Z, A.W / B.W};
#endif
    return result;
}
inline Vec4 DivVec4Flt(Vec4 A, float B)
{
#ifdef USE_SSE
    const __m128 BVec4 = _mm_set1_ps(B);
    const Vec4 result = {.m128 = _mm_div_ps(A.m128, BVec4)};
#else
    const Vec4 result = {A.X / B, A.Y / B, A.Z / B, A.W / B};
#endif
    return result;
}

inline float Vec4Dot(Vec4 A, Vec4 B)
{
    Vec4 mult = MulVec4(A, B);
    return mult.X + mult.Y + mult.Z + mult.W;
}
inline float Vec4Length(Vec4 A)
{
    return sqrtf(Vec4Dot(A, A));
}
inline Vec4 Vec4Max(Vec4 A, Vec4 B)
{
#ifdef USE_SSE
    const Vec4 result = {.m128 = _mm_max_ps(A.m128, B.m128)};
#else
    const Vec4 result = {fmaxf(A.X, B.X), fmaxf(A.Y, B.Y), fmaxf(A.Z, B.Z), fmaxf(A.W, B.W)};
#endif
    return result;
}
inline Vec4 Vec4Min(Vec4 A, Vec4 B)
{
#ifdef USE_SSE
    const Vec4 result = {.m128 = _mm_min_ps(A.m128, B.m128)};
#else
    const Vec4 result = {fminf(A.X, B.X), fminf(A.Y, B.Y), fminf(A.Z, B.Z), fminf(A.W, B.W)};
#endif
    return result;
}