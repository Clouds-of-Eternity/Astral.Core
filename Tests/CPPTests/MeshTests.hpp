#pragma once

#include "CPPTests/ACAssert.h"
#include "LineTriangulator.hpp"
#include "Scope.hpp"
#include <stdio.h>
#include "DataStream.hpp"

inline bool TriangulateToSVG(LineTriangulationInput *input, text path, float lineWidth)
{
    FILE *fs = fopen(path, "w");
    if (fs != NULL)
    {
        //get array lengths
        u32 verticesCount = 0;
        u32 trianglesCount = 0;
        u32 indicesCount = 0;
        LineTriangulator_GetCounts(input, &verticesCount, &trianglesCount, &indicesCount);
        ACASSERT(verticesCount > 0);
        ACASSERT(trianglesCount > 0);
        ACASSERT(indicesCount > 0);

        //prepare arrays
        collections::Array<Maths::Vec3> vertices = collections::Array<Maths::Vec3>(GetCAllocator(), verticesCount);
        collections::Array<u32> indices = collections::Array<u32>(GetCAllocator(), indicesCount);
        Scope(collections::Array<Maths::Vec3>, vertices);
        Scope(collections::Array<u32>, indices);

        //triangulate
        LineTriangulationVertexArray vertexArray = {};
        vertexArray.ptr = vertices.data;
        vertexArray.length = vertices.length;
        vertexArray.structSize = sizeof(Maths::Vec3);
        vertexArray.positionVectorSize = sizeof(Maths::Vec3);
        ACASSERT(LineTriangulator_MapVertices(input, vertexArray, Maths::Vec2(1.0f), lineWidth, true));
        ACASSERT(LineTriangulator_MapIndices(input, indices));

        IDataStream stream = GetWriteFileDataStream(fs, true);
        //<polygon points="100,10 150,190 50,190" style="fill:lime;stroke:purple;stroke-width:3" />
        stream.WriteText("<svg height=\"500\" width=\"500\" xmlns=\"http://www.w3.org/2000/svg\">\n");
        for (u32 i = 0; i < trianglesCount; i++)
        {
            text color = NULL;
            u32 colorIndex = i % 2;
            if (colorIndex == 0)
            {
                color = "red";
            }
            else
            {
                color = "blue";
            }
            stream.WriteText("    <polygon points=\"");
            Maths::Point2 vertex1 = vertices[indices[i * 3 + 0]].ToVector2().ToPoint2();
            Maths::Point2 vertex2 = vertices[indices[i * 3 + 1]].ToVector2().ToPoint2();
            Maths::Point2 vertex3 = vertices[indices[i * 3 + 2]].ToVector2().ToPoint2();
            stream.WriteFormatted("%i,%i %i,%i %i,%i", vertex1.X, vertex1.Y, vertex2.X, vertex2.Y, vertex3.X, vertex3.Y);
            stream.WriteFormatted("\" style=\"fill:%s;\"/>\n", color);
        }
        stream.WriteText("</svg>");

        stream.Flush();
        stream.deinit();
        return true;
    }
    return false;
}

inline bool TestLineTriangulation()
{
    printf(" * Running TestLineTriangulation...\n");


    {
        LineTriangulationInput input = LineTriangulationInput(GetCAllocator());
        input.circumferenceVertices = 2;
        input.segmented = false;
        input.maxPointsIsListCapacity = false;
        Scope(LineTriangulationInput, input);

        //plot the line y=16sin(pi * 0.5x)
        for (u32 x = 0; x < 30; x++)
        {
            input.points.Add(Maths::Vec3(x * 16.0f, 250.0f + 16.0f * sinf(x * 0.2f * PI), 0.0f));
        }

        if (!TriangulateToSVG(&input, "LineTriangulationTest1.svg", 8.0f))
        {
            return false;
        }
    }
    return true;
}

inline void RunMeshTests()
{
    ACTestFunc tests[] = {
        TestLineTriangulation
    };

    AC_RUN_TESTS(RunMeshTests, tests);
}