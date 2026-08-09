#pragma once
#include <stdio.h>
#include "CPPTests/ACAssert.h"
#include "Box.hpp"

inline bool TestBoxIntersects()
{
    printf(" * Running TestBoxIntersects...\n");

    Box boxes[4] = {
        Box(-0.5f, -5.0f, 2.0f, 3.0f),
        Box(1.0f, -3.7f, 5.0f, 0.5f),
        Box(4.0f, 4.0f, 4.0f, 4.0f),
        Box(3.5f, -3.5f, 1.0f, 8.0f)};
    // test results:
    // (0, 0): 1
    // (0, 1): 1
    // (0, 2): 0
    // (0, 3): 0
    // (1, 0): 1
    // (1, 1): 1
    // (1, 2): 0
    // (1, 3): 1
    // (2, 0): 0
    // (2, 1): 0
    // (2, 2): 1
    // (2, 3): 1
    // (3, 0): 0
    // (3, 1): 1
    // (3, 2): 1
    // (3, 3): 1
    const u8 intersectionStates[] = {
        1,
        1,
        0,
        0,
        1,
        1,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        1,
        1,
        1};
    const usize boxesCount = sizeof(boxes) / sizeof(Box);

    u32 checkIndex = 0;
    for (u32 i = 0; i < boxesCount; i++)
    {
        for (u32 j = 0; j < boxesCount; j++)
        {
            bool intersects = boxes[i].Intersects(boxes[j]);
            ACASSERT((u32)intersects == intersectionStates[checkIndex]);
            checkIndex++;
            // printf(" *     (%u, %u): %u\n", i, j, intersects ? 1 : 0);
        }
    }

    return true;
}

inline void RunGeometryTests()
{
    ACTestFunc tests[] = {
        TestBoxIntersects};

    AC_RUN_TESTS(RunGeometryTests, tests);
}