#pragma once
#include "./Quaternion.h"

#define MATRIX4x4_DECOMPOSE_EPSILON 0.0001f

typedef struct Matrix4x4
{
    union
    {
        struct
        {
            //Row 1
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
        Vec4 rows[4];
    };
} Matrix4x4;

inline Matrix4x4 Matrix4x4_FromArray(const float *values)
{
    Matrix4x4 matrix;
    matrix.rows[0] = CreateVec4Ptr(&values[0]);
    matrix.rows[1] = CreateVec4Ptr(&values[4]);
    matrix.rows[2] = CreateVec4Ptr(&values[8]);
    matrix.rows[3] = CreateVec4Ptr(&values[12]);

    return matrix;
}
inline Matrix4x4 Matrix4x4_Empty()
{
    Matrix4x4 matrix;
    matrix.rows[0] = CreateVec4Scalar(0.0f);
    matrix.rows[1] = CreateVec4Scalar(0.0f);
    matrix.rows[2] = CreateVec4Scalar(0.0f);
    matrix.rows[3] = CreateVec4Scalar(0.0f);

    return matrix;
}
inline Matrix4x4 Matrix4x4_Identity()
{
    Matrix4x4 matrix;
    matrix.rows[0] = CreateVec4(1.0f, 0.0f, 0.0f, 0.0f);
    matrix.rows[1] = CreateVec4(0.0f, 1.0f, 0.0f, 0.0f);
    matrix.rows[2] = CreateVec4(0.0f, 0.0f, 1.0f, 0.0f);
    matrix.rows[3] = CreateVec4(0.0f, 0.0f, 0.0f, 1.0f);

    return matrix;
}
inline float Matrix4x4_GetDeterminant(const Matrix4x4 *self)
{
    float a = self->rows[0].X, b = self->rows[0].Y, c = self->rows[0].Z, d = self->rows[0].W;
    float e = self->rows[1].X, f = self->rows[1].Y, g = self->rows[1].Z, h = self->rows[1].W;
    float i = self->rows[2].X, j = self->rows[2].Y, k = self->rows[2].Z, l = self->rows[2].W;
    float m = self->rows[3].X, n = self->rows[3].Y, o = self->rows[3].Z, p = self->rows[3].W;

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

inline bool Matrix4x4_Invert(const Matrix4x4 *self, Matrix4x4 *output)
{
    Matrix4x4 result;

    float a = self->M11, b = self->M12, c = self->M13, d = self->M14;
    float e = self->M21, f = self->M22, g = self->M23, h = self->M24;
    float i = self->M31, j = self->M32, k = self->M33, l = self->M34;
    float m = self->M41, n = self->M42, o = self->M43, p = self->M44;

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
        return false;
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

    *output = result;
    return true;
}
inline Matrix4x4 Matrix4x4_Transpose(const Matrix4x4 *self)
{
    Matrix4x4 result;

    result.M11 = self->M11;
    result.M12 = self->M21;
    result.M13 = self->M31;
    result.M14 = self->M41;
    result.M21 = self->M12;
    result.M22 = self->M22;
    result.M23 = self->M32;
    result.M24 = self->M42;
    result.M31 = self->M13;
    result.M32 = self->M23;
    result.M33 = self->M33;
    result.M34 = self->M43;
    result.M41 = self->M14;
    result.M42 = self->M24;
    result.M43 = self->M34;
    result.M44 = self->M44;

    return result;
}
inline Vec4 Matrix4x4_Transform4(const Matrix4x4 *self, Vec4 vec4)
{
    return CreateVec4(
    vec4.X * self->M11 + vec4.Y * self->M21 + vec4.Z * self->M31 + vec4.W * self->M41,
    vec4.X * self->M12 + vec4.Y * self->M22 + vec4.Z * self->M32 + vec4.W * self->M42,
    vec4.X * self->M13 + vec4.Y * self->M23 + vec4.Z * self->M33 + vec4.W * self->M43,
    vec4.X * self->M14 + vec4.Y * self->M24 + vec4.Z * self->M34 + vec4.W * self->M44);
}
inline Vec3 Matrix4x4_Transform3(const Matrix4x4 *self, Vec3 vec3)
{
    Vec4 asVec4 = Vec3to4(vec3, 1.0f);
    asVec4 = Matrix4x4_Transform4(self, asVec4);
    return CreateVec3(asVec4.X / asVec4.W, asVec4.Y / asVec4.W, asVec4.Z / asVec4.W);
}

#ifdef USE_SSE
static inline __m128 LincombSSE(const __m128 A, const Matrix4x4 *B)
{
    __m128 result;
    result = _mm_mul_ps(_mm_shuffle_ps(A, A, 0x00), B->rows[0].m128);
    result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(A, A, 0x55), B->rows[1].m128));
    result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(A, A, 0xaa), B->rows[2].m128));
    result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(A, A, 0xff), B->rows[3].m128));
    return result;
}
#endif

inline Matrix4x4 MulMatrix4x4(const Matrix4x4 *self, const Matrix4x4 *other)
{
    Matrix4x4 m;
#ifdef USE_SSE
    m.rows[0].m128 = LincombSSE(self->rows[0].m128, other);
    m.rows[1].m128 = LincombSSE(self->rows[1].m128, other);
    m.rows[2].m128 = LincombSSE(self->rows[2].m128, other);
    m.rows[3].m128 = LincombSSE(self->rows[3].m128, other);
#else

    // First row
    m.M11 = self->M11 * other->M11 + self->M12 * other->M21 + self->M13 * other->M31 + self->M14 * other->M41;
    m.M12 = self->M11 * other->M12 + self->M12 * other->M22 + self->M13 * other->M32 + self->M14 * other->M42;
    m.M13 = self->M11 * other->M13 + self->M12 * other->M23 + self->M13 * other->M33 + self->M14 * other->M43;
    m.M14 = self->M11 * other->M14 + self->M12 * other->M24 + self->M13 * other->M34 + self->M14 * other->M44;

    // Seco d row
    m.M21 = self->M21 * other->M11 + self->M22 * other->M21 + self->M23 * other->M31 + self->M24 * other->M41;
    m.M22 = self->M21 * other->M12 + self->M22 * other->M22 + self->M23 * other->M32 + self->M24 * other->M42;
    m.M23 = self->M21 * other->M13 + self->M22 * other->M23 + self->M23 * other->M33 + self->M24 * other->M43;
    m.M24 = self->M21 * other->M14 + self->M22 * other->M24 + self->M23 * other->M34 + self->M24 * other->M44;

    // Thir  row
    m.M31 = self->M31 * other->M11 + self->M32 * other->M21 + self->M33 * other->M31 + self->M34 * other->M41;
    m.M32 = self->M31 * other->M12 + self->M32 * other->M22 + self->M33 * other->M32 + self->M34 * other->M42;
    m.M33 = self->M31 * other->M13 + self->M32 * other->M23 + self->M33 * other->M33 + self->M34 * other->M43;
    m.M34 = self->M31 * other->M14 + self->M32 * other->M24 + self->M33 * other->M34 + self->M34 * other->M44;

    // Four h row
    m.M41 = self->M41 * other->M11 + self->M42 * other->M21 + self->M43 * other->M31 + self->M44 * other->M41;
    m.M42 = self->M41 * other->M12 + self->M42 * other->M22 + self->M43 * other->M32 + self->M44 * other->M42;
    m.M43 = self->M41 * other->M13 + self->M42 * other->M23 + self->M43 * other->M33 + self->M44 * other->M43;
    m.M44 = self->M41 * other->M14 + self->M42 * other->M24 + self->M43 * other->M34 + self->M44 * other->M44;
#endif
    return m;
}
inline Quaternion Matrix4x4_ToQuaternion(const Matrix4x4 *self)
{
    float trace = self->M11 + self->M22 + self->M33;

    Quaternion q = {};

    if (trace > 0.0f)
    {
        float s = sqrtf(trace + 1.0f);
        q.W = s * 0.5f;
        s = 0.5f / s;
        q.X = (self->M23 - self->M32) * s;
        q.Y = (self->M31 - self->M13) * s;
        q.Z = (self->M12 - self->M21) * s;
    }
    else
    {
        if (self->M11 >= self->M22 && self->M11 >= self->M33)
        {
            float s = sqrtf(1.0f + self->M11 - self->M22 - self->M33);
            float invS = 0.5f / s;
            q.X = 0.5f * s;
            q.Y = (self->M12 + self->M21) * invS;
            q.Z = (self->M13 + self->M31) * invS;
            q.W = (self->M23 - self->M32) * invS;
        }
        else if (self->M22 > self->M33)
        {
            float s = sqrtf(1.0f + self->M22 - self->M11 - self->M33);
            float invS = 0.5f / s;
            q.X = (self->M21 + self->M12) * invS;
            q.Y = 0.5f * s;
            q.Z = (self->M32 + self->M23) * invS;
            q.W = (self->M31 - self->M13) * invS;
        }
        else
        {
            float s = sqrtf(1.0f + self->M33 - self->M11 - self->M22);
            float invS = 0.5f / s;
            q.X = (self->M31 + self->M13) * invS;
            q.Y = (self->M32 + self->M23) * invS;
            q.Z = 0.5f * s;
            q.W = (self->M12 - self->M21) * invS;
        }
    }

    return q;
}
inline bool Matrix4x4_Decompose(const Matrix4x4 *self, Vec3 *outPosition, Vec3 *outScale, Quaternion *outRotation)
{
    Matrix4x4 temp = Matrix4x4_Identity();
    Vec3 canonicalBasis[3] = {
        CreateVec3(1.0f, 0.0f, 0.0f),
        CreateVec3(0.0f, 1.0f, 0.0f),
        CreateVec3(0.0f, 0.0f, 1.0f)
    };
    *outPosition = CreateVec3(self->M41, self->M42, self->M43);

    Vec3 *vectorBasis[3] = {
        (Vec3 *)&temp.rows[0],
        (Vec3 *)&temp.rows[1],
        (Vec3 *)&temp.rows[2]
    };
    *(vectorBasis[0]) = *(Vec3 *)&self->rows[0];
    *(vectorBasis[1]) = *(Vec3 *)&self->rows[1];
    *(vectorBasis[2]) = *(Vec3 *)&self->rows[2];

    float scales[3] = {Vec3Length(*vectorBasis[0]), Vec3Length(*vectorBasis[1]), Vec3Length(*vectorBasis[2])};
    uint32_t a;
    uint32_t b;
    uint32_t c;

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

    *vectorBasis[a] = Vec3Normalized(*vectorBasis[a]); // Vec3::Normalize(*vectorBasis[a]);
    if (scales[b] < MATRIX4x4_DECOMPOSE_EPSILON)
    {
        uint32_t cc;
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

        *vectorBasis[b] = Vec3Cross(*vectorBasis[a], canonicalBasis[cc]);
    }

    *vectorBasis[b] = Vec3Normalized(*vectorBasis[b]);

    if (scales[c] < MATRIX4x4_DECOMPOSE_EPSILON)
    {
        *vectorBasis[c] = Vec3Cross(*vectorBasis[a], *vectorBasis[b]);
    }

    *vectorBasis[c] = Vec3Normalized(*vectorBasis[c]);

    float det = Matrix4x4_GetDeterminant(&temp);

    // use Kramer's rule to check for handedness of coordinate system
    if (det < 0.0f)
    {
        // switch coordinate system by negating the scale and inverting the basis vector on the x-axis
        scales[a] = -scales[a];
        *vectorBasis[a] = NegateVec3(*vectorBasis[a]);

        det = -det;
    }

    det -= 1.0f;
    det *= det;

    bool result;

    if (MATRIX4x4_DECOMPOSE_EPSILON < det)
    {
        // Non-SRT matrix encountered
        //rotation = Quaternion.Identity;
        *outRotation = Quaternion_Identity();
        result = false;
    }
    else
    {
        // generate the quaternion from the matrix
        //rotation = Quaternion.CreateFromRotationMatrix(matTemp.AsM4x4());
        *outRotation = Matrix4x4_ToQuaternion(&temp); // Quaternion::FromRotationMatrix(temp);
        result = true;
    }
    *outScale = *(Vec3 *)scales;
    return result;
}

inline Matrix4x4 Matrix4x4_CreateTranslation(float X, float Y, float Z)
{
    float m[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            X, Y, Z, 1.0f
    };
    return Matrix4x4_FromArray(m);
}
inline Matrix4x4 Matrix4x4_CreateTranslationVec3(Vec3 translation)
{
    return Matrix4x4_CreateTranslation(translation.X, translation.Y, translation.Z);
}

inline Matrix4x4 Matrix4x4_CreateScale(float X, float Y, float Z)
{
    float m[16] = {
            X, 0.0f, 0.0f, 0.0f,
            0.0f, Y, 0.0f, 0.0f,
            0.0f, 0.0f, Z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    };
    return Matrix4x4_FromArray(m);
}
inline Matrix4x4 Matrix4x4_CreateScaleVec3(Vec3 scale)
{
    return Matrix4x4_CreateScale(scale.X, scale.Y, scale.Z);
}

inline Matrix4x4 Matrix4x4_CreateRotationSC(float s, float c)
{
    float m[16] = {
        c, s, 0.0f, 0.0f,
        -s, c, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return Matrix4x4_FromArray(m);
}
inline Matrix4x4 Matrix4x4_CreateRotationZ(float rotationZRadians)
{
    float s = sinf(rotationZRadians);
    float c = cosf(rotationZRadians);

    return Matrix4x4_CreateRotationSC(s, c);
}
inline Matrix4x4 Matrix4x4_CreatePerspectiveFOV(float FOV, float aspectRatioXoverY, float nearPlaneDistance, float farPlaneDistance)
{
    float yScale = 1.0f / tanf(FOV * 0.5f);
    float xScale = yScale / aspectRatioXoverY;
    float m[16] = {
        xScale, 0.0f, 0.0f, 0.0f,
        0.0f, yScale, 0.0f, 0.0f,
        0.0f, 0.0f, farPlaneDistance / (nearPlaneDistance - farPlaneDistance), -1.0f,
        0.0f, 0.0f, nearPlaneDistance * farPlaneDistance / (nearPlaneDistance - farPlaneDistance), 0.0f
    };
    return Matrix4x4_FromArray(m);
}
inline Matrix4x4 Matrix4x4_CreateOrthographic(float width, float height, float nearZ, float farZ)
{
    float m[16] = {
        2.0f / width, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / height, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / (nearZ - farZ), 0.0f,
        0.0f, 0.0f, nearZ / (nearZ - farZ), 1.0f
    };
    return Matrix4x4_FromArray(m);
}
inline Matrix4x4 Matrix4x4_CreateOrthographicOffset(Vec3 offset, float width, float height, float nearZ, float farZ)
{
    float m[16] = {
        2.0f / width, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / height, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / (farZ - nearZ), 0.0f,
        offset.X, offset.Y, offset.Z - nearZ / (farZ - nearZ), 1.0f
    };
    return Matrix4x4_FromArray(m);
}
inline Matrix4x4 Matrix4x4_CreateLookAt(Vec3 cameraPosition, Vec3 cameraTarget, Vec3 up)
{
    const Vec3 negCameraPos = NegateVec3(cameraPosition);
    const Vec3 zAxis = NegateVec3(cameraTarget);
    const Vec3 xAxis = Vec3Normalized(Vec3Cross(up, zAxis));
    const Vec3 yAxis = Vec3Cross(zAxis, xAxis);

    const Vec3 cmPos = CreateVec3(Vec3Dot(xAxis, negCameraPos), Vec3Dot(yAxis, negCameraPos), Vec3Dot(zAxis, negCameraPos));

    float m[16] = {
        xAxis.X, yAxis.X, zAxis.X, 0.0f,
        xAxis.Y, yAxis.Y, zAxis.Y, 0.0f,
        xAxis.Z, yAxis.Z, zAxis.Z, 0.0f,
        cmPos.X, cmPos.Y, cmPos.Z, 1.0f
    };
    return Matrix4x4_FromArray(m);
}
inline Matrix4x4 Matrix4x4_CreateWorld(Vec3 position, Vec3 forward, Vec3 up)
{
    Vec3 zAxis = Vec3Normalized(NegateVec3(forward));
    Vec3 xAxis = Vec3Normalized(Vec3Cross(up, zAxis));
    Vec3 yAxis = Vec3Cross(zAxis, xAxis);

    float m[16] = {
        xAxis.X, yAxis.X, zAxis.X, 0.0f,
        xAxis.Y, yAxis.Y, zAxis.Y, 0.0f,
        xAxis.Z, yAxis.Z, zAxis.Z, 0.0f,
        position.X, position.Y, position.Z, 1.0f
    };
    return Matrix4x4_FromArray(m);
}
inline Matrix4x4 Matrix4x4_CreateFromQuaternion(Quaternion quaternion)
{
    const float xx = quaternion.X * quaternion.X;
    const float yy = quaternion.Y * quaternion.Y;
    const float zz = quaternion.Z * quaternion.Z;

    const float xy = quaternion.X * quaternion.Y;
    const float wz = quaternion.Z * quaternion.W;
    const float xz = quaternion.Z * quaternion.X;
    const float wy = quaternion.Y * quaternion.W;
    const float yz = quaternion.Y * quaternion.Z;
    const float wx = quaternion.X * quaternion.W;

    float m[16] = {
        1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f,
        2.0f * (xy - wz), 1.0f - 2.0f * (zz + xx), 2.0f * (yz + wx), 0.0f,
        2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (yy + xx), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return Matrix4x4_FromArray(m);
}
inline Matrix4x4 Matrix4x4_CreateFromTransform(Vec3 position, Vec3 scale, Quaternion rotation)
{
    Matrix4x4 result;
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