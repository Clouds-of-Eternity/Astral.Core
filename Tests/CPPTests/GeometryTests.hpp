#pragma once
#include <stdio.h>
#include "CPPTests/ACAssert.h"
#include "Box.hpp"
#include "Line2D.hpp"
#include "Partitions2D.hpp"
#include "Sorting.hpp"
#include "UTF8Utils.hpp"
#include "CliCanvas.hpp"

inline i8 CompareVec2X(Maths::Vec2 &A, Maths::Vec2 &B)
{
    if (A.X < B.X)
    {
        return 1;
    }
    else if (A.X > B.X)
    {
        return -1;
    }
    else return 0;
}
/*inline void RasterizePolygonFromVertices(u32 index, Maths::Vec2 *vertices, u32 numVertices, CliCanvas canvas)
{
    //scanline
    if (numVertices == 0)
    {
        return;
    }

    Maths::Vec2 min = Maths::Vec2(AC_FloatMax);
    Maths::Vec2 max = Maths::Vec2(AC_FloatMin);
    for (u32 i = 0; i <= numVertices; i++)
    {
        min = Maths::Vec2::Min(vertices[i], min);
        max = Maths::Vec2::Max(vertices[i], max);
    }
    collections::List<Maths::Vec2> points = collections::List<Maths::Vec2>(GetCAllocator());
    
    Maths::Point2 minPoint = Maths::Point2((i32)floorf(min.X * resolution) + canvasArea.X, (i32)floorf(min.Y * resolution) + canvasArea.Y);
    Maths::Point2 maxPoint = Maths::Point2((i32)ceilf(max.X * resolution) + canvasArea.X, (i32)ceilf(max.Y * resolution) + canvasArea.Y);
    for (u32 i = minPoint.Y; i <= maxPoint.Y; i++)
    {
        float y = (i + 0.5f - canvasArea.Y) / resolution;
        Line2D ray = Line2D(Maths::Vec2(min.X, min.Y + y), Maths::Vec2(max.X + 1.0f, min.Y + y));
        for (u32 j = 0; j <= numVertices; j++)
        {
            Line2D line = Line2D(vertices[j], vertices[(j + 1) % numVertices]);
            Maths::Vec2 intersectionPoint = {};
            if (ray.Intersects(line, &intersectionPoint))
            {
                points.Add(intersectionPoint);
            }
        }

        if (points.count % 2 == 0 && points.count > 0)
        {
            TimSort<Maths::Vec2>(points.ptr, points.count, &CompareVec2X);
            assert(points[0].X < points[1].X);


        }
        points.Clear();
    }
}*/
inline void PrintBoxes(const Box *boxes, u32 numBoxes, CliCanvas &canvas)//, float resolution)
{
    if (numBoxes == 0)
    {
        printf("NIL\n");
        return;
    }

    for (u32 j = 0; j < canvas.height; j++)
    {
        for (u32 i = 0; i < canvas.width; i++)
        {
            Maths::Vec2 pos = Maths::Vec2((i + 0.5f) / canvas.resolution, (j + 0.5f) / canvas.resolution) + Maths::Vec2(canvas.worldSpaceX, canvas.worldSpaceY);
            u32 number = 0;
            for (u32 c = 0; c < numBoxes; c++)
            {
                if (boxes[c].Contains(pos))
                {
                    number = (c + 1);
                }
            }
            if (number > 0)
            {
                char numberChar;
                if (number < 10)
                {
                    numberChar = '0' + number;
                }
                else if (number - 10 < 26)
                {
                    numberChar = 'a' + (number - 10);
                }
                else if (number - 36 < 26)
                {
                    numberChar = 'A' + (number - 36);
                }
                else assert(false);
                canvas.Set(i, j, numberChar);
            }
        }
    }
}
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
inline bool TestBoxTouches()
{
    printf(" * Running TestBoxTouches...\n");

    Box box = Box(0.0f, 0.0f, 5.0f, 5.0f);
    Box checkAgainst[] = {
        Box(0.0f, 0.0f, 5.0f, 5.0f),
        Box(5.0f, 0.0f, 5.0f, 5.0f),
        Box(5.001f, 0.0f, 5.0f, 5.0f),
        Box(5.01f, 0.0f, 5.0f, 5.0f),
        Box(4.9f, 0.0f, 5.0f, 5.0f),

        Box(-2.0f, 2.0f, 2.0f, 2.0f),
        Box(-1.0f, 5.0f, 1.0f, 1.0f),
        Box(-1.0f, -2.0f, 2.0f, 2.0f),

        Box(0.0f, 4.0f, 2.0f, 3.0f),
        Box(0.0f, 5.0f, 2.0f, 3.0f)
    };
    const i8 intendedResults[] = {
        -1,
        2,
        2,
        -1,
        -1,

        0,
        0, //touching on corner = either (0, 1, 2 or 3) in priority
        1,

        -1, //left corners equal but ultimately overlaps
        3 //left corners equal, top/bottom touch (should not be a special case)
    };

    u32 boxesCount = sizeof(checkAgainst) / sizeof(Box);
    for (u32 i = 0; i < boxesCount; i++)
    {
        i8 touchingEdge = box.Touches(checkAgainst[i], 0.001f);
        //printf("%u: %i\n", i, touchingEdge);
        ACASSERT(touchingEdge == intendedResults[i])
        // if (box.Touches(checkAgainst[i], 0.001f))
        // {
        //     printf(" *     Box touches with other box %u\n", i);
        // }
        // else printf(" *     Box DOES NOT with other box %u\n", i);
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

        //PrintBoxes(outputs, numOutputs, 2.0f);
        //printf("\n");
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
inline bool TestMergeBoxes()
{
    printf(" * Running TestMergeBoxes...\n");
    collections::List<Box> outputBoxes = collections::List<Box>(GetCAllocator(), 64);
    collections::List<Box> inputBoxes = collections::List<Box>(GetCAllocator(), 64);

    CliCanvas canvas = CliCanvas(GetCAllocator(), 0, 0, 8.0f, 8.0f, 2.0f, '.');
    //fully filled in, should merge into single square
    {
        canvas.Clear();
        for (u32 x = 0; x < 8; x++)
        {
            for (u32 y = 0; y < 8; y++)
            {
                inputBoxes.Add(Box(x, y, 1.0f, 1.0f));
            }
        }

        Partitions2D_MergeBoxes(inputBoxes.ptr, inputBoxes.count, outputBoxes, 0.001f, true, true);
        ACASSERT(outputBoxes.count == 1);

        PrintBoxes(outputBoxes.ptr, outputBoxes.count, canvas);

        canvas.Print();
        printf("\n");

        inputBoxes.Clear();
        outputBoxes.Clear();
    }
    {
        canvas.Clear();
        for (u32 x = 0; x < 4; x++)
        {
            for (u32 y = 0; y < 4; y++)
            {
                inputBoxes.Add(Box(x, y, 1.0f, 1.0f));
            }
        }
        inputBoxes.Add(Box(7.0f, -1.0f, 1.0f, 5.0f));
        inputBoxes.Add(Box(5.5f, -1.0f, 0.6f, 5.0f));
        
        for (u32 x = 0; x < 8; x++)
        {
            if (x % 2 == 0)
            {
                inputBoxes.Add(Box(x, 4.0f, 1.0f, 2.0f));
                inputBoxes.Add(Box(x, 6.0f, 1.0f, 2.0f));
            }
            else
            {
                inputBoxes.Add(Box(x, 4.0f, 1.0f, 4.0f));
            }
        }

        Partitions2D_MergeBoxes(inputBoxes.ptr, inputBoxes.count, outputBoxes, 0.001f, true, true);
        ACASSERT(outputBoxes.count == 4);

        PrintBoxes(outputBoxes.ptr, outputBoxes.count, canvas);

        canvas.Print();
        printf("\n");
        printf("\n");

        inputBoxes.Clear();
        outputBoxes.Clear();
    }

    canvas.deinit();
    outputBoxes.deinit();
    return true;
}

inline void RunGeometryTests()
{
    ACTestFunc tests[] = {
        TestBoxIntersects,
        TestBoxTouches,
        TestClipBoxes,
        TestMergeBoxes
    };

    AC_RUN_TESTS(RunGeometryTests, tests);
}