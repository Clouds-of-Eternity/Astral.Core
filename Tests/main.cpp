#define SOKOL_TIME_IMPL
//#define FORCE_NO_INTRINSICS

#define ASTRALCORE_JSON_IMPL
#include "Json.hpp"
#include "Maths/Util.hpp"

#include "CPPTests/CollectionsTests.hpp"
//#include "local/PerfTests.hpp"

i32 main()
{
    //stm_setup();

    #if DEBUG
    printf("Running in DEBUG mode...\n");
    #else
    printf("Running in RELEASE mode...\n");
    #endif

    RunCollectionsTests();
    //FindInString();
    // ParseJson5MB();
    // FindInString();
    
    return 0;
}