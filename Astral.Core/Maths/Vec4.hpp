#pragma once
#define VEC4_H
#include "Maths/Vec2.hpp"
#include "Maths/Vec3.hpp"
#include "Maths/Matrix4x4.hpp"
#include <math.h>
#include "Maths/simd.h"

namespace Maths
{
	struct Vec4
	{
        union
        {
            struct {
                float X, Y, Z, W;
            };
#ifdef USE_SSE
            __m128 asM128;
#endif
        };

        Vec4()
        {
#ifdef USE_SSE
            asM128 = _mm_set1_ps(0.0f);
#else
            X = 0.0f;
            Y = 0.0f;
            Z = 0.0f;
            W = 0.0f;
#endif
        }
        Vec4(float unit)
        {
#ifdef USE_SSE
            asM128 = _mm_set1_ps(unit);
#else
            X = unit;
            Y = unit;
            Z = unit;
            W = unit;
#endif
        }
        Vec4(float x, float y, float z, float w)
        {
#ifdef USE_SSE
            asM128 = _mm_set_ps(w, z, y, x);
#else
            X = x;
            Y = y;
            Z = z;
            W = w;
#endif
        }
        Vec4(Vec2 vec2, float z, float w)
        {
#ifdef USE_SSE
            asM128 = _mm_set_ps(w, z, vec2.Y, vec2.X);
#else
            X = vec2.X;
            Y = vec2.Y;
            Z = z;
            W = w;
#endif
        }
        Vec4(Vec3 vec3, float w)
        {
#ifdef USE_SSE
            asM128 = _mm_set_ps(w, vec3.Z, vec3.Y, vec3.X);
#else
            X = vec3.X;
            Y = vec3.Y;
            Z = vec3.Z;
            W = w;
#endif
        }
#ifdef USE_SSE
        Vec4(__m128 m128)
        {
            asM128 = m128;
        }
#endif

        //addition operators
        inline Vec4 operator+(Vec4 other)
        {
#ifdef USE_SSE
            Vec4 result;
            result.asM128 = _mm_add_ps(this->asM128, other.asM128);
            return result;
#else

            Vec4 result = *this;
            result.X += other.X;
            result.Y += other.Y;
            result.Z += other.Z;
            result.W += other.W;
            return result;
#endif
        }
        inline void operator+=(Vec4 other)
        {
#ifdef USE_SSE
            this->asM128 = _mm_add_ps(this->asM128, other.asM128);
#else
            X += other.X;
            Y += other.Y;
            Z += other.Z;
            W += other.W;
#endif
        }

        //subtraction operators
        inline Vec4 operator-(Vec4 other)
        {
#ifdef USE_SSE
            Vec4 result;
            result.asM128 = _mm_sub_ps(this->asM128, other.asM128);
            return result;
#else

            Vec4 result = *this;
            result.X -= other.X;
            result.Y -= other.Y;
            result.Z -= other.Z;
            result.W -= other.W;
            return result;
#endif
        }
        inline void operator-=(Vec4 other)
        {
#ifdef USE_SSE
            this->asM128 = _mm_sub_ps(this->asM128, other.asM128);
#else
            X -= other.X;
            Y -= other.Y;
            Z -= other.Z;
            W -= other.W;
#endif
        }

        //multiplication operators
        inline Vec4 operator*(Vec4 other)
        {
#ifdef USE_SSE
            Vec4 result;
            result.asM128 = _mm_mul_ps(this->asM128, other.asM128);
            return result;
#else
            Vec4 result;
            result.X = X * other.X;
            result.Y = Y * other.Y;
            result.Z = Z * other.Z;
            result.W = W * other.W;
            return result;
#endif
        }
        inline void operator*=(Vec4 other)
        {
#ifdef USE_SSE
            asM128 = _mm_mul_ps(this->asM128, other.asM128);
#else
            X *= other.X;
            Y *= other.Y;
            Z *= other.Z;
            W *= other.W;
#endif
        }
        inline Vec4 operator*(float other)
        {
            return Vec4(X * other, Y * other, Z * other, W * other);
        }
        inline void operator*=(float other)
        {
            X *= other;
            Y *= other;
            Z *= other;
            W *= other;
        }

        //division operators
        inline Vec4 operator/(Vec4 other)
        {
#ifdef USE_SSE
            Vec4 result;
            result.asM128 = _mm_div_ps(this->asM128, other.asM128);
            return result;
#else
            Vec4 result;
            result.X = X / other.X;
            result.Y = Y / other.Y;
            result.Z = Z / other.Z;
            result.W = W / other.W;
            return result;
#endif
        }
        inline void operator/=(Vec4 other)
        {
#ifdef USE_SSE
            asM128 = _mm_div_ps(this->asM128, other.asM128);
#else
            X /= other.X;
            Y /= other.Y;
            Z /= other.Z;
            W /= other.W;
#endif
        }
        inline Vec4 operator/(float other)
        {
            float mult = 1.0f / other;
            return Vec4(X * mult, Y * mult, Z * mult, W * mult);
        }
        inline void operator/=(float other)
        {
            float mult = 1.0f / other;
            X *= mult;
            Y *= mult;
            Z *= mult;
            W *= mult;
        }

        inline Vec4 operator-()
        {
#ifdef USE_SSE
            return Vec4(_mm_mul_ps(asM128, _mm_set1_ps(-1.0f)));
#else
            return Vec4(-X, -Y, -Z, -W);
#endif
        }

        inline Vec4 Transform(Matrix4x4 matrix)
        {
            return Vec4(
            X * matrix.M11 + Y * matrix.M21 + Z * matrix.M31 + W * matrix.M41,
            X * matrix.M12 + Y * matrix.M22 + Z * matrix.M32 + W * matrix.M42,
            X * matrix.M13 + Y * matrix.M23 + Z * matrix.M33 + W * matrix.M43,
            X * matrix.M14 + Y * matrix.M24 + Z * matrix.M34 + W * matrix.M44);
        }
        inline Vec4 operator*(Matrix4x4 matrix)
        {
            return this->Transform(matrix);
        }
        inline Vec4 operator*=(Matrix4x4 matrix)
        {
            *this = this->Transform(matrix);
        }

        float Length()
        {
            return sqrtf(Dot(*this, *this));
        }

        static inline Vec4 Lerp4(Vec4 A, Vec4 B, Vec4 amount)
        {
#ifdef USE_SSE
            __m128 sub = _mm_sub_ps(_mm_set1_ps(1.0f), amount.asM128);
            __m128 mul = _mm_mul_ps(A.asM128, sub);
            __m128 fma = _mm_add_ps(_mm_mul_ps(B.asM128, amount.asM128), mul);
            return Vec4(fma);
#else
            Vec4 result;
            result.X = A.X + (B.X - A.X) * amount.X;
            result.Y = A.Y + (B.Y - A.Y) * amount.Y;
            result.Z = A.Z + (B.Z - A.Z) * amount.Z;
            result.W = A.W + (B.W - A.W) * amount.W;
            return result;
#endif
        }
        static inline Vec4 Lerp(Vec4 A, Vec4 B, float amount)
        {
#ifdef USE_SSE
            return Lerp4(A, B, Vec4(amount));
#else
            Vec4 result;
            result.X = A.X + (B.X - A.X) * amount;
            result.Y = A.Y + (B.Y - A.Y) * amount;
            result.Z = A.Z + (B.Z - A.Z) * amount;
            result.W = A.W + (B.W - A.W) * amount;
            return result;
#endif
        }
        static inline float Dot(Vec4 A, Vec4 B)
        {
            return A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
        }

        /*static inline Vec4 Cross(Vec4 A, Vec4 B)
        {
            __m128 tmp0, tmp1, tmp2, tmp3, result;
            tmp0 = _mm_shuffle_ps(A.asM128, A.asM128, _MM_SHUFFLE(3, 0, 2, 1));
            tmp1 = _mm_shuffle_ps(B.asM128, B.asM128, _MM_SHUFFLE(3, 1, 0, 2));
            tmp2 = _mm_shuffle_ps(A.asM128, A.asM128, _MM_SHUFFLE(3, 1, 0, 2));
            tmp3 = _mm_shuffle_ps(B.asM128, B.asM128, _MM_SHUFFLE(3, 0, 2, 1));
            result = _mm_mul_ps(tmp0, tmp1);
            result = _mm_sub_ps(result, _mm_mul_ps(tmp2, tmp3));//sseMSub(tmp2, tmp3, result);
            return Vec4(result);
        }*/
        
        static inline Vec4 Min(Vec4 A, Vec4 B)
        {
#ifdef USE_SSE
            return Vec4(_mm_min_ps(A.asM128, B.asM128));
#else
            return Vec4(fminf(A.X, B.X), fminf(A.Y, B.Y), fminf(A.Z, B.Z), fminf(A.W, B.W));
#endif
        }
        static inline Vec4 Max(Vec4 A, Vec4 B)
        {
#ifdef USE_SSE
            return Vec4(_mm_max_ps(A.asM128, B.asM128));
#else
            return Vec4(fmaxf(A.X, B.X), fmaxf(A.Y, B.Y), fmaxf(A.Z, B.Z), fmaxf(A.W, B.W));
#endif
        }
        inline bool operator==(Vec4 other)
        {
            return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
        }
        inline bool operator!=(Vec4 other)
        {
            return X != other.X || Y != other.Y || Z != other.Z || W != other.W;
        }
	};

    //typedef Maths_Vec4 Vec4;

}