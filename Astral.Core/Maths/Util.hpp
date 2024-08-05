#pragma once
#include "math.h"
#include "Maths/Vec2.hpp"
#include "Linxc.h"

#define PI 3.1415927f
#define DOUBLEPI 3.14159265358979323846
constexpr float Degree2Radian = 3.1415927f / 180.0f;
constexpr float Radian2Degree = 180.0f / 3.1415927f;

#define LERP(A, B, amount) A + (B - A) * amount
#define FloatMin -3.40282347E+38f
#define FloatMax 3.40282347E+38f
#define U64Min 0
#define U64Max 18446744073709551615llu
#define U32Min 0
#define U32Max 4294967295
#define U16Min 0
#define U16Max 65535
#define U8Min 0
#define U8Max 255
#define I64Min -9223372036854775808ll
#define I64Max 9223372036854775807ll
#define I32Min -2147483648
#define I32Max 2147483647
#define I16Min -32768
#define I16Max 32767
#define I8Min -128
#define I8Max 127

#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a < b) ? a : b)

namespace Maths
{
	u64 inline rotl64(u64 value, u64 amount)
	{
		amount %= 64;
		return (value << amount) | (value >> (64 - amount));
	}
	u32 inline rotl32(u32 value, u32 amount)
	{
		amount %= 32;
		return (value << amount) | (value >> (32 - amount));
	}
	u64 inline rotr64(u64 value, u64 amount)
	{
		amount %= 64;
		return (value >> amount) | (value << (64 - amount));
	}
	u32 inline rotr32(u32 value, u32 amount)
	{
		amount %= 32;
		return (value >> amount) | (value << (32 - amount));
	}
	float inline LengthdirX(float len, float dirInRadians)
	{
		return cosf(dirInRadians) * len;
	}
	float inline LengthdirY(float len, float dirInRadians)
	{
		return sinf(dirInRadians) * len;
	}
	Vec2 inline Lengthdir(float len, float dirInRadians)
	{
		return Vec2(cosf(dirInRadians) * len, sinf(dirInRadians) * len);
	}
	float inline DirectionTowards(Vec2 A, Vec2 B)
	{
		return atan2f(B.Y - A.Y, B.X - A.X);
	}
	float inline DirectionTowards(float AX, float AY, float BX, float BY)
	{
		return atan2f(BY - AY, BX - AX);
	}
	i32 inline Sign(float A)
	{
		if (A < 0.0f)
		{
			return -1;
		}
		else if (A > 0.0f)
		{
			return 1;
		}
		return 0;
	}
	i32 inline Sign(i32 A)
	{
		if (A < 0)
		{
			return -1;
		}
		else if (A > 0)
		{
			return 1;
		}
		return 0;
	}
	float inline QuadraticStep(float A, float B, float amount)
	{
		float equation = Sign(B - A) * amount * amount + (B < A ? 1.0f : 0.0f);
		return A + (B - A) * equation;
	}
	float inline BarycentricCoords(float A, float B, float C, float amount1, float amount2)
	{
		return A + (B - A) * amount1 + (C - A) * amount2;
	}
	float inline Clamp(float min, float max, float value)
	{
		if (value < min)
		{
			return min;
		}
		else if (value > max)
		{
			return max;
		}
		return value;
	}
	i32 inline Clamp(i32 min, i32 max, i32 value)
	{
		if (value < min)
		{
			return min;
		}
		else if (value > max)
		{
			return max;
		}
		return value;
	}
	float inline Distance(float A, float B)
	{
		return fabsf(B - A);
	}
	float inline Hermite(float v1, float t1, float v2, float t2, float amount)
	{
		double result;
		double amtCubed = amount * amount * amount;
		double amtSquared = amount * amount;

		if (amount == 0.0f)
		{
			result = v1;
		}
		else if (amount == 1.0f)
		{
			result = v2;
		}
		else
		{
			result = (2 * v1 - 2 * v2 + t2 + t1) * amtCubed +
				(3 * v2 - 3 * v1 - 2 * t1 - t2) * amtSquared +
				t1 * amount +
				v1;
		}
		return (float)result;
	}
	float inline ModuloFloat(float A, float B)
	{
		i32 n = (i32)floorl(A / B);
		return A - (n * B);
	}
	float inline WrapAngle(float angle)
	{
		if ((angle > -PI) && (angle <= PI))
			return angle;
		angle = ModuloFloat(angle, PI * 2.0f);
		if (angle <= -PI)
			return angle + PI * 2.0f;
		if (angle > PI)
			return angle - PI * 2.0f;
		return angle;
	}
	float inline LerpFloat(float current, float designated, float amt)
	{
		return current + (designated - current) * amt;
	}
	float inline LerpAngle(float current, float designated, float amt)
	{
		if (designated < current)
		{
			if (designated + (180.0f * Degree2Radian) < current)
			{
				float distance = (2 * PI) - fabsf(designated - current);
				return WrapAngle(current + (amt * distance));
			}
			else
			{
				float distance = current - designated;
				return WrapAngle(current - (distance * amt));
			}
		}
		else if (current < designated)
		{
			if (designated - (180.0f * Degree2Radian) > current)
			{
				float distance = (2 * PI) - fabsf(designated - current);
				return WrapAngle(current - (amt * distance));
			}
			else
			{
				float distance = designated - current;
				return WrapAngle(current + (distance * amt));
			}
		}
		else return current;
	}
	constexpr float Approach(float value, float toApproach, float amount)
	{
		return value + (toApproach - value) * amount;
	}
	inline float ApproachAngle(float curAngle, float targetAngle, float maxChange)
	{
		curAngle = WrapAngle(curAngle);
		targetAngle = WrapAngle(targetAngle);
		if (curAngle < targetAngle)
		{
			if (targetAngle - curAngle > PI)
			{
				curAngle += PI * 2.0f;
			}
		}
		else if (curAngle - targetAngle > PI)
		{
			curAngle -= PI * 2.0f;
		}
		curAngle += Clamp(targetAngle - curAngle, -maxChange, maxChange);
		return WrapAngle(curAngle);
	}
	inline static Vec2 RotateAbout(Vec2 thisPoint, Vec2 pivot, float s, float c)
	{
		if (thisPoint == pivot)
		{
			return thisPoint;
		}

		thisPoint -= pivot;
		float num = thisPoint.X * c - thisPoint.Y * s;
		float num2 = thisPoint.X * s + thisPoint.Y * c;
		thisPoint = Vec2(num + pivot.X, num2 + pivot.Y);
		return thisPoint;
	}
	inline static Vec2 RotateAbout(Vec2 thisPoint, Vec2 pivot, float angle)
	{
		if (thisPoint == pivot)
		{
			return thisPoint;
		}

		float s = sinf(angle);
		float c = cosf(angle);
		return RotateAbout(thisPoint, pivot, s, c);
	}
	inline Vec2 TripleProduct(Vec2 A, Vec2 B, Vec2 C)
	{
		float num = A.X * B.Y - A.Y * B.X;
		return Vec2((0.0f - C.Y) * num, C.X * num);
	}
}