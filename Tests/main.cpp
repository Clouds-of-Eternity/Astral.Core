#define SOKOL_TIME_IMPL
//#define FORCE_NO_INTRINSICS

#define ASTRALCORE_JSON_IMPL
#include "Json.hpp"
#include "CPPTests/PerfTests.hpp"

i32 main()
{
    stm_setup();

    #if DEBUG
    printf("Running in DEBUG mode...\n");
    #else
    printf("Running in RELEASE mode...\n");
    #endif

    #ifdef USE_SSE
    printf("Running with SSE intrinsics support.\n");
    #else
    printf("Running with SIMD disabled.\n");
    #endif

    //FindInString();
    ParseJson5MB();
    
    return 0;
}