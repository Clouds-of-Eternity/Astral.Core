#pragma once
#include "Linxc.h"

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct
    {
        float X;
        float Y;
    } vec2;

    typedef struct
    {
        float X;
        float Y;
        float Z;
    } vec3;

    typedef struct
    {
        float X;
        float Y;
        float Z;
        float W;
    } vec4;

    typedef struct
    {
        i32 X;
        i32 Y;
    } point2;

    typedef struct
    {
        float X;
        float Y;
        float Width;
        float Height;
    } box;

    typedef struct
    {
        u8 R;
        u8 G;
        u8 B;
        u8 A;
    } col32;

    typedef struct
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
    } mat4x4;
#ifdef __cplusplus
}
#endif