#pragma once
#include <math.h>
#include "Maths/simd.h"
#include "Maths/Util.hpp"
#include "Maths/Vec4.hpp"
#include "Maths/Quaternion.hpp"
#include "option.hpp"

#include "assert.h"

#define MATRIX4x4_DECOMPOSE_EPSILON 0.0001f

namespace Maths
{
	struct Matrix4x4
	{
		//Row 1
		union
		{
			struct
			{
				float M11;
				float M12;
				float M13;
				float M14;
				//Row 2
				float M21;
				float M22;
				float M23;
				float M24;
				//Row 3
				float M31;
				float M32;
				float M33;
				float M34;
				//Row 4
				float M41;
				float M42;
				float M43;
				float M44;
			};
			struct
			{
				Maths::Vec4 X;
				Maths::Vec4 Y;
				Maths::Vec4 Z;
				Maths::Vec4 W;
			} asVec4s;
#ifdef USE_SSE
			struct
			{
				__m128 m128_row1;
				__m128 m128_row2;
				__m128 m128_row3;
				__m128 m128_row4;
			};
#endif
			struct
			{
				float row1[4];
				float row2[4];
				float row3[4];
				float row4[4];
			};
		};

		Matrix4x4()
		{

		}
		Matrix4x4(float fill)
		{
#ifdef USE_SSE
			m128_row1 = _mm_set_ps1(fill);
			m128_row2 = _mm_set_ps1(fill);
			m128_row3 = _mm_set_ps1(fill);
			m128_row4 = _mm_set_ps1(fill);
#else
			M11 = M12 = M13 = M14 = fill;
			M21 = M22 = M23 = M24 = fill;
			M31 = M32 = M33 = M34 = fill;
			M41 = M42 = M43 = M44 = fill;
#endif
		}
		Matrix4x4(float* m)
		{
#ifdef USE_SSE
			m128_row1 = _mm_load_ps(m);//_mm_set_ps(m[0], m[1], m[2], m[3]);
			m128_row2 = _mm_load_ps(m + 4);//_mm_set_ps(m[4], m[5], m[6], m[7]);
			m128_row3 = _mm_load_ps(m + 8);//_mm_set_ps(m[8], m[9], m[10], m[11]);
			m128_row4 = _mm_load_ps(m + 12);//_mm_set_ps(m[12], m[13], m[14], m[15]);
#else
			M11 = m[0];
			M12 = m[1];
			M13 = m[2];
			M14 = m[3];

			M21 = m[4];
			M22 = m[5];
			M23 = m[6];
			M24 = m[7];

			M31 = m[8];
			M32 = m[9];
			M33 = m[10];
			M34 = m[11];

			M41 = m[12];
			M42 = m[13];
			M43 = m[14];
			M44 = m[15];
#endif
		}
		inline float GetDeterminant()
		{
			float a = asVec4s.X.X, b = asVec4s.X.Y, c = asVec4s.X.Z, d = asVec4s.X.W;
			float e = asVec4s.Y.X, f = asVec4s.Y.Y, g = asVec4s.Y.Z, h = asVec4s.Y.W;
			float i = asVec4s.Z.X, j = asVec4s.Z.Y, k = asVec4s.Z.Z, l = asVec4s.Z.W;
			float m = asVec4s.W.X, n = asVec4s.W.Y, o = asVec4s.W.Z, p = asVec4s.W.W;

			float kp_lo = k * p - l * o;
			float jp_ln = j * p - l * n;
			float jo_kn = j * o - k * n;
			float ip_lm = i * p - l * m;
			float io_km = i * o - k * m;
			float in_jm = i * n - j * m;

			return a * (f * kp_lo - g * jp_ln + h * jo_kn) -
					b * (e * kp_lo - g * ip_lm + h * io_km) +
					c * (e * jp_ln - f * ip_lm + h * in_jm) -
					d * (e * jo_kn - f * io_km + g * in_jm);
		}
		inline Quaternion ToQuaternion()
		{
            float trace = M11 + M22 + M33;
 
            Quaternion q = Quaternion();
 
            if (trace > 0.0f)
            {
                float s = sqrtf(trace + 1.0f);
                q.W = s * 0.5f;
                s = 0.5f / s;
                q.X = (M23 - M32) * s;
                q.Y = (M31 - M13) * s;
                q.Z = (M12 - M21) * s;
            }
            else
            {
                if (M11 >= M22 && M11 >= M33)
                {
                    float s = sqrtf(1.0f + M11 - M22 - M33);
                    float invS = 0.5f / s;
                    q.X = 0.5f * s;
                    q.Y = (M12 + M21) * invS;
                    q.Z = (M13 + M31) * invS;
                    q.W = (M23 - M32) * invS;
                }
                else if (M22 > M33)
                {
                    float s = sqrtf(1.0f + M22 - M11 - M33);
                    float invS = 0.5f / s;
                    q.X = (M21 + M12) * invS;
                    q.Y = 0.5f * s;
                    q.Z = (M32 + M23) * invS;
                    q.W = (M31 - M13) * invS;
                }
                else
                {
                    float s = sqrtf(1.0f + M33 - M11 - M22);
                    float invS = 0.5f / s;
                    q.X = (M31 + M13) * invS;
                    q.Y = (M32 + M23) * invS;
                    q.Z = 0.5f * s;
                    q.W = (M12 - M21) * invS;
                }
            }
 
            return q;
		}
		inline option<Matrix4x4> Invert()
		{
			float a = M11, b = M12, c = M13, d = M14;
			float e = M21, f = M22, g = M23, h = M24;
			float i = M31, j = M32, k = M33, l = M34;
			float m = M41, n = M42, o = M43, p = M44;

			float kp_lo = k * p - l * o;
			float jp_ln = j * p - l * n;
			float jo_kn = j * o - k * n;
			float ip_lm = i * p - l * m;
			float io_km = i * o - k * m;
			float in_jm = i * n - j * m;

			float a11 = +(f * kp_lo - g * jp_ln + h * jo_kn);
			float a12 = -(e * kp_lo - g * ip_lm + h * io_km);
			float a13 = +(e * jp_ln - f * ip_lm + h * in_jm);
			float a14 = -(e * jo_kn - f * io_km + g * in_jm);

			float det = a * a11 + b * a12 + c * a13 + d * a14;

			if (fabsf(det) <= 0.0f)
			{
				return option<Matrix4x4>();
			}

			float invDet = 1.0f / det;

			Matrix4x4 result;
			result.M11 = a11 * invDet;
			result.M21 = a12 * invDet;
			result.M31 = a13 * invDet;
			result.M41 = a14 * invDet;

			result.M12 = -(b * kp_lo - c * jp_ln + d * jo_kn) * invDet;
			result.M22 = +(a * kp_lo - c * ip_lm + d * io_km) * invDet;
			result.M32 = -(a * jp_ln - b * ip_lm + d * in_jm) * invDet;
			result.M42 = +(a * jo_kn - b * io_km + c * in_jm) * invDet;

			float gp_ho = g * p - h * o;
			float fp_hn = f * p - h * n;
			float fo_gn = f * o - g * n;
			float ep_hm = e * p - h * m;
			float eo_gm = e * o - g * m;
			float en_fm = e * n - f * m;

			result.M13 = +(b * gp_ho - c * fp_hn + d * fo_gn) * invDet;
			result.M23 = -(a * gp_ho - c * ep_hm + d * eo_gm) * invDet;
			result.M33 = +(a * fp_hn - b * ep_hm + d * en_fm) * invDet;
			result.M43 = -(a * fo_gn - b * eo_gm + c * en_fm) * invDet;

			float gl_hk = g * l - h * k;
			float fl_hj = f * l - h * j;
			float fk_gj = f * k - g * j;
			float el_hi = e * l - h * i;
			float ek_gi = e * k - g * i;
			float ej_fi = e * j - f * i;

			result.M14 = -(b * gl_hk - c * fl_hj + d * fk_gj) * invDet;
			result.M24 = +(a * gl_hk - c * el_hi + d * ek_gi) * invDet;
			result.M34 = -(a * fl_hj - b * el_hi + d * ej_fi) * invDet;
			result.M44 = +(a * fk_gj - b * ek_gi + c * ej_fi) * invDet;

			return option<Matrix4x4>(result);
		}
		inline bool IsIdentity()
		{
			return M11 == 1.0f && M22 == 1.0f && M33 == 1.0f && M44 == 1.0f &&
				M12 == 0.0f && M13 == 0.0f && M14 == 0.0f &&
				M21 == 0.0f && M23 == 0.0f && M24 == 0.0f &&
				M31 == 0.0f && M32 == 0.0f && M34 == 0.0f &&
				M41 == 0.0f && M42 == 0.0f && M43 == 0.0f;
		}
		inline Matrix4x4 Transpose()
		{
            Matrix4x4 result;

            result.M11 = M11;
            result.M12 = M21;
            result.M13 = M31;
            result.M14 = M41;
            result.M21 = M12;
            result.M22 = M22;
            result.M23 = M32;
            result.M24 = M42;
            result.M31 = M13;
            result.M32 = M23;
            result.M33 = M33;
            result.M34 = M43;
            result.M41 = M14;
            result.M42 = M24;
            result.M43 = M34;
            result.M44 = M44;

            return result;
		}

		inline static Matrix4x4 Identity()
		{
			float m[16] = {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
			};
			return Matrix4x4(&m[0]);
		}
		inline static Matrix4x4 CreateFromTransform(Maths::Vec3 position, Maths::Vec3 scale, Maths::Quaternion rotation)
		{
			Maths::Matrix4x4 result;
			float *lm = &result.M11;
			float tx = position.X;
            float ty = position.Y;
            float tz = position.Z;

            float qx = rotation.X;
            float qy = rotation.Y;
            float qz = rotation.Z;
            float qw = rotation.W;

            float sx = scale.X;
            float sy = scale.Y;
            float sz = scale.Z;

            lm[0] = (1 - 2 * qy * qy - 2 * qz * qz) * sx;
            lm[1] = (2 * qx * qy + 2 * qz * qw) * sx;
            lm[2] = (2 * qx * qz - 2 * qy * qw) * sx;
            lm[3] = 0.0f;

            lm[4] = (2 * qx * qy - 2 * qz * qw) * sy;
            lm[5] = (1 - 2 * qx * qx - 2 * qz * qz) * sy;
            lm[6] = (2 * qy * qz + 2 * qx * qw) * sy;
            lm[7] = 0.0f;

            lm[8] = (2 * qx * qz + 2 * qy * qw) * sz;
            lm[9] = (2 * qy * qz - 2 * qx * qw) * sz;
            lm[10] = (1 - 2 * qx * qx - 2 * qy * qy) * sz;
            lm[11] = 0.0f;

            lm[12] = tx;
            lm[13] = ty;
            lm[14] = tz;
            lm[15] = 1.0f;

            return result;
		}
		inline static Matrix4x4 CreateTranslation(float X, float Y, float Z)
		{
			float m[16] = {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					X, Y, Z, 1.0f
			};
			return Matrix4x4(m);
		}
		inline static Matrix4x4 CreateTranslation(Vec3 pos)
		{
			float m[16] = {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					pos.X, pos.Y, pos.Z, 1.0f
			};
			return Matrix4x4(&m[0]);
		}
		inline static Matrix4x4 CreateScale(float X, float Y, float Z)
		{
			float m[16] = {
					X, 0.0f, 0.0f, 0.0f,
					0.0f, Y, 0.0f, 0.0f,
					0.0f, 0.0f, Z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
			};
			return Matrix4x4(&m[0]);
		}
		inline static Matrix4x4 CreateScale(Vec3 scale)
		{
			float m[16] = {
					scale.X, 0.0f, 0.0f, 0.0f,
					0.0f, scale.Y, 0.0f, 0.0f,
					0.0f, 0.0f, scale.Z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
			};
			return Matrix4x4(&m[0]);
		}
		inline static Matrix4x4 CreateScale(float scale)
		{
			return CreateScale(scale, scale, scale);
		}
		inline static Matrix4x4 CreateRotationZ(float s, float c)
		{
			float m[16] = {
				c, s, 0.0f, 0.0f,
				-s, c, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
			return Matrix4x4(&m[0]);
		}
		inline static Matrix4x4 CreateRotationZ(float angleRadians)
		{
			return CreateRotationZ(sinf(angleRadians), cosf(angleRadians));
		}
		inline static Matrix4x4 CreateFromAxisAngle(Vec3 axis, float angle)
		{
			float x = axis.X, y = axis.Y, z = axis.Z;
			float sa = sinf(angle);
			float ca = cosf(angle);
			float xx = x * x;
			float yy = y * y;
			float zz = z * z;
			float xy = x * y;
			float xz = x * z;
			float yz = y * z;

			float m[16] = {
				xx + ca * (1.0f - xx), xy - ca * xy + sa * z, xz - ca * xz - sa * y, 0.0f,
				xy - ca * xy - sa * z, yy + ca * (1.0f - yy), yz - ca * yz + sa * x, 0.0f,
				xz - ca * xz + sa * y, yz - ca * yz - sa * x, zz + ca * (1.0f - zz), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
			return Matrix4x4(&m[0]);
		}
		inline static Matrix4x4 CreatePerspectiveFOV(float FOV, float aspectRatioXoverY, float nearPlaneDistance, float farPlaneDistance)
		{
			assert(FOV > 0.0f && FOV < PI && nearPlaneDistance > 0.0f && farPlaneDistance > 0.0f && farPlaneDistance > nearPlaneDistance);

			float yScale = 1.0f / tanf(FOV * 0.5f);
			float xScale = yScale / aspectRatioXoverY;
			float m[16] = {
				xScale, 0.0f, 0.0f, 0.0f,
				0.0f, yScale, 0.0f, 0.0f,
				0.0f, 0.0f, farPlaneDistance / (nearPlaneDistance - farPlaneDistance), -1.0f,
				0.0f, 0.0f, nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance), 0.0f
			};
			return Matrix4x4(m);
		}
		inline static Matrix4x4 CreateOrthographic(float width, float height, float nearZ, float farZ)
		{
			float m[16] = {
				2.0f / width, 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f / height, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f / (nearZ - farZ), 0.0f,
				0.0f, 0.0f, nearZ / (nearZ - farZ), 1.0f
			};
			return Matrix4x4(m);
		}
		inline static Matrix4x4 CreateOrthographicOffset(Maths::Vec3 offset, float width, float height, float nearZ, float farZ)
		{
			float m[16] = {
				2.0f / width, 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f / height, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f / (farZ - nearZ), 0.0f,
				offset.X, offset.Y, offset.Z - nearZ / (farZ - nearZ), 1.0f
			};
			return Matrix4x4(m);
		}
		inline static Matrix4x4 CreateLookAt(Vec3 cameraPosition, Vec3 cameraTarget, Vec3 up)
		{
			Vec3 zAxis = -cameraTarget; //(cameraTarget - cameraPosition).Normalized();
			Vec3 xAxis = Vec3::Cross(up, zAxis).Normalized();
			Vec3 yAxis = Vec3::Cross(zAxis, xAxis);

			Vec3 cmPos = Vec3(Vec3::Dot(xAxis, -cameraPosition), Vec3::Dot(yAxis, -cameraPosition), Vec3::Dot(zAxis, -cameraPosition));

			float m[16] = {
				xAxis.X, yAxis.X, zAxis.X, 0.0f,
				xAxis.Y, yAxis.Y, zAxis.Y, 0.0f,
				xAxis.Z, yAxis.Z, zAxis.Z, 0.0f,
				cmPos.X, cmPos.Y, cmPos.Z, 1.0f
			};
			return Matrix4x4(m);
		}
		inline static Matrix4x4 CreateWorld(Vec3 position, Vec3 forward, Vec3 up)
		{
			Vec3 zAxis = (-forward).Normalized();
			Vec3 xAxis = Vec3::Cross(up, zAxis).Normalized();
			Vec3 yAxis = Vec3::Cross(zAxis, xAxis);

			float m[16] = {
				xAxis.X, yAxis.X, zAxis.X, 0.0f,
				xAxis.Y, yAxis.Y, zAxis.Y, 0.0f,
				xAxis.Z, yAxis.Z, zAxis.Z, 0.0f,
				position.X, position.Y, position.Z, 1.0f
			};
			return Matrix4x4(m);
		}
		inline static Matrix4x4 CreateFromQuaternion(Quaternion quaternion)
		{
			float xx = quaternion.X * quaternion.X;
			float yy = quaternion.Y * quaternion.Y;
			float zz = quaternion.Z * quaternion.Z;

			float xy = quaternion.X * quaternion.Y;
			float wz = quaternion.Z * quaternion.W;
			float xz = quaternion.Z * quaternion.X;
			float wy = quaternion.Y * quaternion.W;
			float yz = quaternion.Y * quaternion.Z;
			float wx = quaternion.X * quaternion.W;

			float m[16] = {
				1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f,
				2.0f * (xy - wz), 1.0f - 2.0f * (zz + xx), 2.0f * (yz + wx), 0.0f,
				2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (yy + xx), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
			return Matrix4x4(m);
		}
		inline static Matrix4x4 CreateFromYawPitchRoll(float yaw, float pitch, float roll)
		{
			return Matrix4x4::CreateFromQuaternion(Quaternion::FromYawPitchRoll(yaw, pitch, roll));
		}
		inline static bool Decompose(const Matrix4x4 &self, Maths::Vec3 *outPosition, Maths::Vec3 *outScale, Maths::Quaternion *outRotation)
		{
			Maths::Matrix4x4 temp = Maths::Matrix4x4::Identity();
			Maths::Vec3 canonicalBasis[3] = {
				Maths::Vec3(1.0f, 0.0f, 0.0f),
				Maths::Vec3(0.0f, 1.0f, 0.0f),
				Maths::Vec3(0.0f, 0.0f, 1.0f)
			};
			*outPosition = Maths::Vec3(self.M41, self.M42, self.M43);

			Maths::Vec3 *vectorBasis[3] = {
				(Maths::Vec3 *)temp.row1,
				(Maths::Vec3 *)temp.row2,
				(Maths::Vec3 *)temp.row3
			};
			*(vectorBasis[0]) = *(Maths::Vec3 *)self.row1;
			*(vectorBasis[1]) = *(Maths::Vec3 *)self.row2;
			*(vectorBasis[2]) = *(Maths::Vec3 *)self.row3;

			float scales[3] = {vectorBasis[0]->Length(), vectorBasis[1]->Length(), vectorBasis[2]->Length()};
			u32 a;
			u32 b;
			u32 c;

			//ranking
			float x = scales[0];
			float y = scales[1];
			float z = scales[2];

			if (x < y)
			{
				if (y < z)
				{
					a = 2;
					b = 1;
					c = 0;
				}
				else
				{
					a = 1;

					if (x < z)
					{
						b = 2;
						c = 0;
					}
					else
					{
						b = 0;
						c = 2;
					}
				}
			}
			else
			{
				if (x < z)
				{
					a = 2;
					b = 0;
					c = 1;
				}
				else
				{
					a = 0;

					if (y < z)
					{
						b = 2;
						c = 1;
					}
					else
					{
						b = 1;
						c = 2;
					}
				}
			}

			if (scales[a] < MATRIX4x4_DECOMPOSE_EPSILON)
			{
				*(vectorBasis[a]) = canonicalBasis[a];
			}

			*vectorBasis[a] = (*vectorBasis[a]).Normalized(); // Maths::Vec3::Normalize(*vectorBasis[a]);
			if (scales[b] < MATRIX4x4_DECOMPOSE_EPSILON)
			{
				u32 cc;
				float fAbsX, fAbsY, fAbsZ;

				fAbsX = fabsf(vectorBasis[a]->X);
				fAbsY = fabsf(vectorBasis[a]->Y);
				fAbsZ = fabsf(vectorBasis[a]->Z);

				//ranking
				if (fAbsX < fAbsY)
				{
					if (fAbsY < fAbsZ)
					{
						cc = 0;
					}
					else
					{
						if (fAbsX < fAbsZ)
						{
							cc = 0;
						}
						else
						{
							cc = 2;
						}
					}
				}
				else
				{
					if (fAbsX < fAbsZ)
					{
						cc = 1;
					}
					else
					{
						if (fAbsY < fAbsZ)
						{
							cc = 1;
						}
						else
						{
							cc = 2;
						}
					}
				}

				*vectorBasis[b] = Maths::Vec3::Cross(*vectorBasis[a], canonicalBasis[cc]);
			}

			*vectorBasis[b] = (*vectorBasis[b]).Normalized();
 
			if (scales[c] < MATRIX4x4_DECOMPOSE_EPSILON)
			{
				*vectorBasis[c] = Maths::Vec3::Cross(*vectorBasis[a], *vectorBasis[b]);
			}

			*vectorBasis[c] = (*vectorBasis[c]).Normalized();

			float det = temp.GetDeterminant();

			// use Kramer's rule to check for handedness of coordinate system
			if (det < 0.0f)
			{
				// switch coordinate system by negating the scale and inverting the basis vector on the x-axis
				scales[a] = -scales[a];
				*vectorBasis[a] = -(*vectorBasis[a]);

				det = -det;
			}

			det -= 1.0f;
			det *= det;

			bool result;

			if (MATRIX4x4_DECOMPOSE_EPSILON < det)
			{
				// Non-SRT matrix encountered
				//rotation = Quaternion.Identity;
				*outRotation = Maths::Quaternion::Identity();
				result = false;
			}
			else
			{
				// generate the quaternion from the matrix
				//rotation = Quaternion.CreateFromRotationMatrix(matTemp.AsM4x4());
				*outRotation = temp.ToQuaternion(); // Maths::Quaternion::FromRotationMatrix(temp);
				result = true;
			}
			*outScale = *(Maths::Vec3 *)scales;
			return result;
		}

#ifdef USE_SSE
		static inline __m128 lincomb_SSE(const __m128 &a, const Matrix4x4 &B)
		{
			__m128 result;
			result = _mm_mul_ps(_mm_shuffle_ps(a, a, 0x00), B.m128_row1);
			result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(a, a, 0x55), B.m128_row2));
			result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xaa), B.m128_row3));
			result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xff), B.m128_row4));
			return result;
		}
#endif
		inline Matrix4x4 operator*(const Matrix4x4 &other)
		{
#ifdef USE_SSE
			Maths::Matrix4x4 m;
			m.m128_row1 = lincomb_SSE(m128_row1, other);
			m.m128_row2 = lincomb_SSE(m128_row2, other);
			m.m128_row3 = lincomb_SSE(m128_row3, other);
			m.m128_row4 = lincomb_SSE(m128_row4, other);
			return m;
#else
			Matrix4x4 m;

			// First row
			m.M11 = M11 * other.M11 + M12 * other.M21 + M13 * other.M31 + M14 * other.M41;
			m.M12 = M11 * other.M12 + M12 * other.M22 + M13 * other.M32 + M14 * other.M42;
			m.M13 = M11 * other.M13 + M12 * other.M23 + M13 * other.M33 + M14 * other.M43;
			m.M14 = M11 * other.M14 + M12 * other.M24 + M13 * other.M34 + M14 * other.M44;

			// Second row
			m.M21 = M21 * other.M11 + M22 * other.M21 + M23 * other.M31 + M24 * other.M41;
			m.M22 = M21 * other.M12 + M22 * other.M22 + M23 * other.M32 + M24 * other.M42;
			m.M23 = M21 * other.M13 + M22 * other.M23 + M23 * other.M33 + M24 * other.M43;
			m.M24 = M21 * other.M14 + M22 * other.M24 + M23 * other.M34 + M24 * other.M44;

			// Third row
			m.M31 = M31 * other.M11 + M32 * other.M21 + M33 * other.M31 + M34 * other.M41;
			m.M32 = M31 * other.M12 + M32 * other.M22 + M33 * other.M32 + M34 * other.M42;
			m.M33 = M31 * other.M13 + M32 * other.M23 + M33 * other.M33 + M34 * other.M43;
			m.M34 = M31 * other.M14 + M32 * other.M24 + M33 * other.M34 + M34 * other.M44;

			// Fourth row
			m.M41 = M41 * other.M11 + M42 * other.M21 + M43 * other.M31 + M44 * other.M41;
			m.M42 = M41 * other.M12 + M42 * other.M22 + M43 * other.M32 + M44 * other.M42;
			m.M43 = M41 * other.M13 + M42 * other.M23 + M43 * other.M33 + M44 * other.M43;
			m.M44 = M41 * other.M14 + M42 * other.M24 + M43 * other.M34 + M44 * other.M44;

			return m;
#endif
		}
		inline Matrix4x4 operator*=(const Matrix4x4 other)
		{
			*this = *this * other;
			return *this;
		}

		inline bool operator==(const Matrix4x4 other)
		{
			return M11 == other.M11 && M22 == other.M22 && M33 == other.M33 && M44 == other.M44 &&
				M12 == other.M12 && M13 == other.M13 && M14 == other.M14 &&
				M21 == other.M21 && M23 == other.M23 && M24 == other.M24 &&
				M31 == other.M31 && M32 == other.M32 && M34 == other.M34 &&
				M41 == other.M41 && M42 == other.M42 && M43 == other.M43;
		}
		inline bool operator!=(const Matrix4x4 other)
		{
			return (M11 != other.M11 || M12 != other.M12 || M13 != other.M13 || M14 != other.M14 ||
				M21 != other.M21 || M22 != other.M22 || M23 != other.M23 || M24 != other.M24 ||
				M31 != other.M31 || M32 != other.M32 || M33 != other.M33 || M34 != other.M34 ||
				M41 != other.M41 || M42 != other.M42 || M43 != other.M43 || M44 != other.M44);
		}

		inline Vec4 Transform(Vec4 vec4)
		{
            return Vec4(
            vec4.X * M11 + vec4.Y * M21 + vec4.Z * M31 + vec4.W * M41,
            vec4.X * M12 + vec4.Y * M22 + vec4.Z * M32 + vec4.W * M42,
            vec4.X * M13 + vec4.Y * M23 + vec4.Z * M33 + vec4.W * M43,
            vec4.X * M14 + vec4.Y * M24 + vec4.Z * M34 + vec4.W * M44);
		}
        inline Vec3 Transform(Vec3 vec3)
        {
			Maths::Vec4 asVec4 = Maths::Vec4(vec3, 1.0f);
			asVec4 = Transform(asVec4);
			return Vec3(asVec4.X / asVec4.W, asVec4.Y / asVec4.W, asVec4.Z / asVec4.W);
		}
		inline static Matrix4x4 Lerp(const Matrix4x4 A, const Matrix4x4 B, float amount)
		{
#ifdef USE_SSE
			__m128 amountAsM128 = _mm_set_ps1(amount);
			Matrix4x4 m;
			{
				__m128 sub = _mm_sub_ps(_mm_set1_ps(1.0f), amountAsM128);
				__m128 mul = _mm_mul_ps(A.m128_row1, sub);
				m.m128_row1 = _mm_add_ps(_mm_mul_ps(B.m128_row1, amountAsM128), mul);
			}
			{
				__m128 sub = _mm_sub_ps(_mm_set1_ps(1.0f), amountAsM128);
				__m128 mul = _mm_mul_ps(A.m128_row2, sub);
				m.m128_row2 = _mm_add_ps(_mm_mul_ps(B.m128_row2, amountAsM128), mul);
			}
			{
				__m128 sub = _mm_sub_ps(_mm_set1_ps(1.0f), amountAsM128);
				__m128 mul = _mm_mul_ps(A.m128_row3, sub);
				m.m128_row3 = _mm_add_ps(_mm_mul_ps(B.m128_row3, amountAsM128), mul);
			}
			{
				__m128 sub = _mm_sub_ps(_mm_set1_ps(1.0f), amountAsM128);
				__m128 mul = _mm_mul_ps(A.m128_row4, sub);
				m.m128_row4 = _mm_add_ps(_mm_mul_ps(B.m128_row4, amountAsM128), mul);
			}
			return m;
#else
			Matrix4x4 m;

			m.row1[0] = AC_LERP(A.row1[0], B.row1[0], amount);
			m.row1[1] = AC_LERP(A.row1[1], B.row1[1], amount);
			m.row1[2] = AC_LERP(A.row1[2], B.row1[2], amount);
			m.row1[3] = AC_LERP(A.row1[3], B.row1[3], amount);

			m.row2[0] = AC_LERP(A.row2[0], B.row2[0], amount);
			m.row2[1] = AC_LERP(A.row2[1], B.row2[1], amount);
			m.row2[2] = AC_LERP(A.row2[2], B.row2[2], amount);
			m.row2[3] = AC_LERP(A.row2[3], B.row2[3], amount);

			m.row3[0] = AC_LERP(A.row3[0], B.row3[0], amount);
			m.row3[1] = AC_LERP(A.row3[1], B.row3[1], amount);
			m.row3[2] = AC_LERP(A.row3[2], B.row3[2], amount);
			m.row3[3] = AC_LERP(A.row3[3], B.row3[3], amount);

			m.row4[0] = AC_LERP(A.row4[0], B.row4[0], amount);
			m.row4[1] = AC_LERP(A.row4[1], B.row4[1], amount);
			m.row4[2] = AC_LERP(A.row4[2], B.row4[2], amount);
			m.row4[3] = AC_LERP(A.row4[3], B.row4[3], amount);

			return m;
#endif
		}
	};
}