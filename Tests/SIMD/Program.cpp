#define SOKOL_TIME_IMPL
#include "../sokol_time.h"
#include "Linxc.h"
#include <stdio.h>

#include "Maths/Matrix4x4.hpp"

#ifndef ITERATIONS
#define ITERATIONS 1000000
#endif

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