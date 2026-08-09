#pragma once
#include "Linxc.h"

def_delegate(ACTestFunc, bool);

#define ACASSERT(expression)                                                                  \
    if (!(expression))                                                                        \
    {                                                                                         \
        printf("Assertion failed! %s @ file %s, line %u\n", #expression, __FILE__, __LINE__); \
        return false;                                                                         \
    }

#define AC_RUN_TESTS(testSuiteName, tests)                           \
    printf("----\nExecuting %s()...\n", #testSuiteName);             \
    const usize total = sizeof(tests) / sizeof(ACTestFunc);          \
    u32 passes = total;                                              \
    for (u32 i = 0; i < total; i++)                                  \
    {                                                                \
        if (!tests[i]())                                             \
        {                                                            \
            passes--;                                                \
        }                                                            \
    }                                                                \
    printf("%s() passed: %u/%llu\n", #testSuiteName, passes, total); \
    if (passes == total)                                             \
    {                                                                \
        printf("\n --#### ALL PASSED ####--\n");                                 \
    }                                                                \
    else                                                             \
    {                                                                \
        printf("\n --!!!! ERROR !!!!--\n");                                \
    }\
