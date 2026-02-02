#define SOKOL_TIME_IMPL
#include "../sokol_time.h"
#include "Linxc.h"
#include <stdio.h>

#include "Maths/Matrix4x4.hpp"

#ifndef ITERATIONS
#define ITERATIONS 100000000
#endif

inline void PrintMatrix(const Maths::Matrix4x4 &mat)
{
    printf("%f %f %f %f\n", mat.M11, mat.M12, mat.M13, mat.M14);
    printf("%f %f %f %f\n", mat.M21, mat.M22, mat.M23, mat.M24);
    printf("%f %f %f %f\n", mat.M31, mat.M32, mat.M33, mat.M34);
    printf("%f %f %f %f\n", mat.M41, mat.M42, mat.M43, mat.M44);
}

i32 main()
{
#ifdef USE_SSE
    printf("Using SSE...\n");
#else
    printf("No SIMD...\n");
#endif

#if defined (__clang__)
    printf("Using Clang...\n");
#elif defined(_MSC_VER)
    printf("Using MSVC...\n");
#endif

    stm_setup();

    printf("Vec4 Addition:\n");
    for (u32 c = 0; c < 5; c++)
    {
        Maths::Vec4 A = Maths::Vec4(0.0f);
        Maths::Vec4 B = Maths::Vec4(1.0f, 0.1f, 0.0f, -0.1f);

        double start = stm_ms(stm_now());
        for (i32 i = 0; i < ITERATIONS; i++)
        {
            A += B;
        }
        double end = stm_ms(stm_now());
        
        printf("   - time taken: %f ms (%f, %f, %f, %f)\n", end - start, A.X, A.Y, A.Z, A.W);
    }

    printf("Vec4 Trigonometry:\n");

    for (u32 c = 0; c < 5; c++)
    {
        Maths::Vec4 A = Maths::Vec4(0.0f);
        Maths::Vec4 B = Maths::Vec4(0.0f);

        double start = stm_ms(stm_now());
        for (i32 i = 0; i < ITERATIONS; i++)
        {
            A += (Maths::Vec4(0.0f, 90.0f, 180.0f, 270.0f) * Degree2Radian).Sine();
            B += (Maths::Vec4(0.0f, 90.0f, 180.0f, 270.0f) * Degree2Radian).Cosine();
        }
        double end = stm_ms(stm_now());
        
        printf("   - time taken: %f ms (%f, %f, %f, %f)\n", end - start, A.X, A.Y, A.Z, A.W);
    }

    printf("Matrix Transform:\n");
    for (u32 c = 0; c < 5; c++)
    {
        Maths::Matrix4x4 A = Maths::Matrix4x4::Identity();

        double start = stm_ms(stm_now());
        for (i32 i = 0; i < ITERATIONS; i++)
        {
            Maths::Matrix4x4 B = Maths::Matrix4x4::CreateFromTransform(Maths::Vec3(1.0f, 0.0f, 0.0f), Maths::Vec3(1.0f), Maths::Quaternion::Identity());
            A = A * B;
        }
        double end = stm_ms(stm_now());
        
        printf("   - time taken: %f ms (%f, %f, %f)\n", end - start, A.M41, A.M42, A.M43);
    }
    return 0;
}