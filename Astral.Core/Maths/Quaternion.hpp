#pragma once
#include <math.h>
#include "Maths/Vec3.hpp"
#include "Maths/simd.h"

namespace Maths
{
	struct Quaternion
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

		inline Quaternion()
		{
			X = 0.0f;
			Y = 0.0f;
			Z = 0.0f;
			W = 1.0f;
		}

		inline Quaternion(float x, float y, float z, float w)
		{
#ifdef USE_SSE
			asM128 = _mm_set_ps(x, y, z, w);
#else
			X = x;
			Y = y;
			Z = z;
			W = w;
#endif
		}
		inline Quaternion(Vec3 vector, float scalar)
		{
#ifdef USE_SSE
			asM128 = _mm_set_ps(vector.X, vector.Y, vector.Z, scalar);
#else
			X = vector.X;
			Y = vector.Y;
			Z = vector.Z;
			W = scalar;
#endif
		}
#ifdef USE_SSE
		inline Quaternion(__m128 m128)
		{
			asM128 = m128;
		}
#endif

		static inline Quaternion Identity()
		{
			return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		}

		inline float Length()
		{
			return sqrtf(X * X + Y * Y + Z * Z + W * W);
		}
		inline float LengthSquared()
		{
			return X * X + Y * Y + Z * Z + W * W;
		}
		inline Quaternion Normalized()
		{
			float oneOverNormalized = 1.0f / sqrtf(X * X + Y * Y + Z * Z + W * W);
#ifdef USE_SSE
			return Quaternion(_mm_mul_ps(asM128, _mm_set1_ps(oneOverNormalized)));
#else
			return Quaternion(
				X * oneOverNormalized,
				Y * oneOverNormalized,
				Z * oneOverNormalized,
				W * oneOverNormalized);
#endif
		}
		static inline Quaternion Normalize(Quaternion quat)
		{
			return quat.Normalized();
		}
		static inline Quaternion Conjugate(Quaternion quat)
		{
			return Quaternion(-quat.X, -quat.Y, -quat.Z, quat.W);
		}
		inline Quaternion Inversed()
		{
			float oneOverLengthSquared = 1.0f / (X * X + Y * Y + Z * Z + W * W);
			return Quaternion(-X * oneOverLengthSquared, -Y * oneOverLengthSquared, -Z * oneOverLengthSquared, W * oneOverLengthSquared);
		}
		static inline Quaternion Inverse(Quaternion quat)
		{
			return quat.Inversed();
		}
		static inline Quaternion FromAxisAngle(Vec3 axis, float angle)
		{
			float s = sinf(angle * 0.5f);
			float c = cosf(angle * 0.5f);
			return Quaternion(axis.X * s, axis.Y * s, axis.Z * s, c);
		}
		static inline Quaternion FromYawPitchRoll(float yaw, float pitch, float roll)
		{
			float sRoll = sinf(roll * 0.5f);
			float cRoll = cosf(roll * 0.5f);
			float sPitch = sinf(pitch * 0.5f);
			float cPitch = cosf(pitch * 0.5f);
			float sYaw = sinf(yaw * 0.5f);
			float cYaw = cosf(yaw * 0.5f);

			return Quaternion(
				cYaw * sPitch * cRoll + sYaw * cPitch * sRoll,
				sYaw * cPitch * cRoll - cYaw * sPitch * sRoll,
				cYaw * cPitch * sRoll - sYaw * sPitch * cRoll,
				cYaw * cPitch * cRoll + sYaw * sPitch * sRoll
			);
		}
		static inline float Dot(Quaternion A, Quaternion B)
		{
			return A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
		}
		static inline Quaternion Lerp(Quaternion A, Quaternion B, float amount)
		{
			float invAmount = 1.0f - amount;
			float dot = A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
			if (dot >= 0.0f)
			{
				return Quaternion(
					invAmount * A.X + amount * B.X,
					invAmount * A.Y + amount * B.Y,
					invAmount * A.Z + amount * B.Z,
					invAmount * A.W + amount * B.W
					).Normalized();
			}
			else
			{
				return Quaternion(
					invAmount * A.X - amount * B.X,
					invAmount * A.Y - amount * B.Y,
					invAmount * A.Z - amount * B.Z,
					invAmount * A.W - amount * B.W
				).Normalized();
			}
		}
		inline Quaternion Concat(Quaternion other)
		{
			float cX = other.Y * Z - other.Z * Y;
			float cY = other.Z * X - other.X * Z;
			float cZ = other.X * Y - other.Y * X;

			float dot = other.X * X + other.Y * Y + other.Z * Z;

			return Quaternion(
				other.X * W + X * other.W + cX, 
				other.Y * W + Y * other.W + cY,
				other.Z * W + Z * other.W + cZ,
				other.W * W - dot
			);
		}
		static inline Quaternion Concat(Quaternion A, Quaternion B)
		{
			return A.Concat(B);
		}
		inline Quaternion Negative()
		{
			return Quaternion(-X, -Y, -Z, -W);
		}
		inline Quaternion Add(Quaternion other)
		{
#ifdef USE_SSE
			return Quaternion(_mm_add_ps(this->asM128, other.asM128));
#else
			return Quaternion(X + other.X, Y + other.Y, Z + other.Z, W + other.W);
#endif
		}
		inline Quaternion Subtract(Quaternion other)
		{
#ifdef USE_SSE
			return Quaternion(_mm_sub_ps(this->asM128, other.asM128));
#else
			return Quaternion(X - other.X, Y - other.Y, Z - other.Z, W - other.W);
#endif
		}
		inline Quaternion Mult(Quaternion other)
		{
			float cX = Y * other.Z - Z * other.Y;
			float cY = Z * other.X - X * other.Z;
			float cZ = X * other.Y - Y * other.X;

			float dot = X * other.X + Y * other.Y + Z * other.Z;

			return Quaternion(
				X * other.W + other.X * W + cX,
				Y * other.W + other.Y * W + cY,
				Z * other.W + other.Z * W + cZ,
				W * other.W - dot
			);
		}
		inline Quaternion Div(Quaternion other)
		{
			float ls = other.X * other.X + other.Y * other.Y + other.Z * other.Z + other.W * other.W;
			float invNorm = 1.0f / ls;

			float q2x = -other.X * invNorm;
			float q2y = -other.Y * invNorm;
			float q2z = -other.Z * invNorm;
			float q2w = other.W * invNorm;

			float cx = Y * q2z - Z * q2y;
			float cy = Z * q2x - X * q2z;
			float cz = X * q2y - Y * q2x;

			float dot = X * q2x + Y * q2y + Z * q2z;

			return Quaternion(
				X * q2w + q2x * W + cx,
				Y * q2w + q2y * W + cy,
				Z * q2w + q2z * W + cz,
				W * q2w - dot
			);
		}
		inline Quaternion operator*(Quaternion other)
		{
			return Mult(other);
		}
		inline void operator*=(Quaternion other)
		{
			*this = Mult(other);
		}
		inline Quaternion operator/(Quaternion other)
		{
			return Div(other);
		}
		inline void operator/=(Quaternion other)
		{
			*this = Div(other);
		}
		inline bool operator==(Quaternion other)
		{
			return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
		}
		inline bool operator!=(Quaternion other)
		{
			return X != other.X || Y != other.Y || Z != other.Z || W != other.W;
		}
	};
}
