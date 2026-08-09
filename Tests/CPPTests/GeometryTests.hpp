#pragma once
#include <stdio.h>
#include "CPPTests/ACAssert.h"
#include "Box.hpp"
#include "Partitions2D.hpp"

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

struct TestClipBoxExpectedResult
{
    Box results[4];
    bool returnVal;
    u8 numResults;
};
inline bool TestClipBoxes()
{
    Box box = Box(0.0f, 0.0f, 5.0f, 5.0f);
    Box clips[] = {
        Box(2.0f, -2.0f, 5.0f, 3.0f), //clips top right -> (0, 0, 2, 1), (0, 1, 5, 4)
        Box(-0.1f, -0.1f, 0.2f, 5.11f), //clips left segment -> (0.1, 0, 4.9, 5)
        Box(0.5f, 4.0f, 4.0f, 2.0f), //clips bottom edge -> (0, 0, 0.5, 5), (0.5, 0, 4, 4), (4.5, 0, 0.5, 5)
        Box(-1.0f, -1.0f, 7.0f, 100.0f), //clips everything -> 0
        Box(5.001f, 0.0f, 5.0f, 5.0f) //clips nothing -> no intersection
    };
    TestClipBoxExpectedResult expectedResults[] = {
        TestClipBoxExpectedResult{{Box(0, 0, 2, 1), Box(0, 1, 5, 4)}, true, 2},
        TestClipBoxExpectedResult{{Box(0.1f, 0.0f, 4.9f, 5.0f)}, true, 1},
        TestClipBoxExpectedResult{{Box(0, 0, 0.5f, 5.0f), Box(0.5f, 0, 4, 4), Box(4.5f, 0, 0.5f, 5.0f)}, true, 3},
        TestClipBoxExpectedResult{{}, true, 0},
        TestClipBoxExpectedResult{{}, false, 0}
    };

    u32 numOutputs;
    Box outputs[4] = {};
    const usize boxesCount = sizeof(clips) / sizeof(Box);

    for (u32 i = 0; i < boxesCount; i++)
    {
        bool result = Partitions2D_ClipBoxBox(box, clips[i], outputs, &numOutputs);
        ACASSERT(result == expectedResults[i].returnVal);
        ACASSERT(numOutputs == expectedResults[i].numResults);
        for (u32 c = 0; c < numOutputs; c++)
        {
            ACASSERT(outputs[c] == expectedResults[i].results[c]);
        }
        // if (Partitions2D_ClipBoxBox(box, clips[i], outputs, &numOutputs))
        // {
        //     printf(" *     Clipped %u, results: %u\n", i, numOutputs);
        //     for (u32 j = 0; j < numOutputs; j++)
        //     {
        //         printf(" *      - %f, %f, %f, %f\n", outputs[j].X, outputs[j].Y, outputs[j].width, outputs[j].height);
        //     }
        // }
        // else
        // {
        //     printf(" *     Clipped %u, no intersection\n", i);
        // }
    }

    return true;
}

inline void RunGeometryTests()
{
    ACTestFunc tests[] = {
        TestBoxIntersects,
        TestClipBoxes
    };

    AC_RUN_TESTS(RunGeometryTests, tests);
}