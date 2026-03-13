#pragma once
#include "./Vectors.h"

typedef Vec4 Quaternion;

#define CreateQuaternion CreateVec4
#define QuaternionEqls Vec4Eqls

inline Quaternion ConjugateQuaternion(Quaternion A)
{
#ifdef USE_SSE
    Quaternion result;
    result.m128 = _mm_mul_ps(A.m128, _mm_setr_ps(-1.0f, -1.0f, -1.0f, 1.0f));
#else
    const Quaternion result = {-A.X, -A.Y, -A.Z, A.W};
#endif
    return result;
}

inline Quaternion Quaternion_Invert(Quaternion A)
{
    const float lengthSquared = Vec4Dot(A, A);
    return DivVec4Flt(ConjugateQuaternion(A), lengthSquared);//Quaternion(-X * oneOverLengthSquared, -Y * oneOverLengthSquared, -Z * oneOverLengthSquared, W * oneOverLengthSquared);
}
inline void QuaternionToAxisAngle(Quaternion self, float *output)
{
    output[3] = acosf(self.W) * 2.0f;
    output[0] = self.X / sinf(output[3] * 0.5f);
    output[1] = self.Y / sinf(output[3] * 0.5f);
    output[2] = self.Z / sinf(output[3] * 0.5f);
}
inline Quaternion MulQuaternion(Quaternion A, Quaternion B)
{
    const float cX = A.Y * B.Z - A.Z * B.Y;
    const float cY = A.Z * B.X - A.X * B.Z;
    const float cZ = A.X * B.Y - A.Y * B.X;

    const float dot = A.X * B.X + A.Y * B.Y + A.Z * B.Z;

    return CreateQuaternion(
        A.X * B.W + B.X * A.W + cX,
        A.Y * B.W + B.Y * A.W + cY,
        A.Z * B.W + B.Z * A.W + cZ,
        A.W * B.W - dot
    );
}

inline Quaternion Quaternion_Identity()
{
    return CreateQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
}
inline Quaternion Quaternion_FromAxisAngle(Vec3 axis, float angle)
{
    float s = sinf(angle * 0.5f);
    float c = cosf(angle * 0.5f);
    return CreateQuaternion(axis.X * s, axis.Y * s, axis.Z * s, c);
}
inline Quaternion Quaternion_FromYawPitchRoll(float yaw, float pitch, float roll)
{
    Vec3 sin3 = CreateVec3(sinf(roll * 0.5f), sinf(pitch * 0.5f), sinf(yaw * 0.5f));
    Vec3 cos3 = CreateVec3(cosf(roll * 0.5f), cosf(pitch * 0.5f), cosf(yaw * 0.5f));

    float sr = sin3.X;
    float sp = sin3.Y;
    float sy = sin3.Z;
    float cr = cos3.X;
    float cp = cos3.Y;
    float cy = cos3.Z;

    Quaternion result;

    result.X = cy * sp * cr + sy * cp * sr;
    result.Y = sy * cp * cr - cy * sp * sr;
    result.Z = cy * cp * sr - sy * sp * cr;
    result.W = cy * cp * cr + sy * sp * sr;

    return result;
}
inline Quaternion Quaternion_FromNormalizedDirection(Vec3 dir)
{
    const Vec3 forward = CreateVec3(1.0f, 0.0f, 0.0f);

    float dot = Vec3Dot(forward, dir);
    Vec3 rotAxis;
    if (dot == 1.0f)
    {
        rotAxis = CreateVec3(0.0f, 0.0f, 1.0f);
    }
    else if (dot == -1.0f)
    {
        rotAxis = CreateVec3(0.0f, 0.0f, -1.0f);
    }
    else rotAxis = Vec3Normalized(Vec3Cross(forward, dir));
    float rotation = acosf(dot);

    return Quaternion_FromAxisAngle(rotAxis, rotation);
}

inline Vec4 QuaternionTransform4(Quaternion self, Vec4 pos)
{
    Quaternion conjugated = ConjugateQuaternion(self);
    Quaternion temp = MulQuaternion(conjugated, CreateQuaternion(pos.X, pos.Y, pos.Z, pos.W));
    temp = MulQuaternion(temp, self);
    return CreateVec4(temp.X, temp.Y, temp.Z, temp.W);
}
inline Vec3 QuaternionTransform3(Quaternion self, Vec3 pos)
{
    Vec4 asVec4 = Vec3to4(pos, 1.0f);
    Vec4 result = QuaternionTransform4(self, asVec4);
    return CreateVec3(result.X / result.W, result.Y / result.W, result.Z / result.W);
}
inline Vec2 QuaternionTransform2(Quaternion self, Vec2 pos)
{
    Vec4 asVec4 = Vec2to4(pos, 0.0f, 1.0f);
    Vec4 result = QuaternionTransform4(self, asVec4);
    return CreateVec2(result.X / result.W, result.Y / result.W);
}