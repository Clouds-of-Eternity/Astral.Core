#pragma once
#include "./Vectors.h"

#define MATRIX4x4_DECOMPOSE_EPSILON 0.0001f

typedef struct
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