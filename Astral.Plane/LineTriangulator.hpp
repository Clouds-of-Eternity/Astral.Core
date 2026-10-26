#pragma once
#include "List.hpp"
#include "Span.hpp"
#include "Maths/Vec4.hpp"

struct LineTriangulationInput
{
    /// @brief The coordinates that make up the line.
    collections::List<Maths::Vec3> points;
    /// @brief If true, 'maxPoints' for all the subsequent operations using this instance as an input
    /// will use points.capacity instead of points.count.
    bool maxPointsIsListCapacity;
    /// @brief If true, all columns of segments will be able to be transformed individually of the
    /// previous and next columns.
    bool segmented;
    /// @brief The number of vertices per column. Must be at least 2.
    u8 circumferenceVertices;
};

// LineRenderer *LineRenderer_Create(Actor onHandle, u32 maxPoints, u8 circumferenceVertices)
// {
//     LineRenderer *result = (LineRenderer *)DEFAULT_ALLOC(sizeof(LineRenderer));
//     *result = LineRenderer();

//     result->onHandle = onHandle;
//     result->circumferenceVertices = circumferenceVertices;
//     LineRenderer_EnsureArrayCapacity(result, maxPoints);
// }
inline void LineRenderer_GetCounts(const LineTriangulationInput *self, u32 *outVerticesCount, u32 *outTrianglesCount, u32 *outIndicesCount)
{
    //if segmented, each segment will have a unique set of both the start and end columnal vertices.
    //Otherwise, the end column will be shared with the next segment's start column, if any.

    //In both scenarios, the index count will remain the same, just that the index values will differ.

    //Example calculation:
    //circumferenceVertices = 2, maxPoints = 3
    //0-2-4
    //|/|/|
    //1-3-5
    //-----
    //segmentsCount = maxPoints - 1 = 3 - 1 = 2
    //If not segmented: vertices = maxPoints * circumferenceVertices = 6 * 4 = 24
    //If segmented: vertices = segmentsCount * circumferenceVertices * 2 = 8
    //total triangles = segmentsCount * (circumferenceVertices - 1) * 2 = 2 * (2 - 1) * 2 = 4
    //total indices = 4 * 3 = 12

    //circumferenceVertices = 4, maxPoints = 6
    //00-04-08-12-16-20
    //|/ |/ |/ |/ |/ |
    //01-05-09-13-17-21
    //|/ |/ |/ |/ |/ |
    //02-06-10-14-18-22
    //|/ |/ |/ |/ |/ |
    //03-07-11-15-19-23
    //-----------------
    //segmentsCount = maxPoints - 1 = 6 - 1 = 5
    //If not segmented: vertices = maxPoints * circumferenceVertices = 6 * 4 = 24
    //If segmented: vertices = segmentsCount * circumferenceVertices * 2 = 5 * 4 * 2 = 40
    //total triangles = segmentsCount * (circumferenceVertices - 1) * 2 = 5 * (4 - 1) * 2 = 5 * 3 * 2 = 30
    //total indices = 30 * 3 = 90

    const u32 maxPoints = self->maxPointsIsListCapacity ? (u32)self->points.capacity : (u32)self->points.count;

    const u32 segmentsCount = maxPoints - 1;
    if (outVerticesCount != NULL)
    {
        if (!self->segmented)
        {
            *outVerticesCount = maxPoints * self->circumferenceVertices;
        }
        else
        {
            *outVerticesCount = segmentsCount * self->circumferenceVertices * 2;
        }
    }
    const u32 totalTriangles = segmentsCount * (self->circumferenceVertices - 1) * 2;
    if (outTrianglesCount != NULL)
    {
        *outTrianglesCount = totalTriangles;
    }
    if (outIndicesCount != NULL)
    {
        *outIndicesCount = totalTriangles * 3;
    }
}
inline bool LineRenderer_MapIndices(const LineTriangulationInput *input, Span<u32> output)
{
    u32 trianglesCount;
    u32 minSpanLength = 0;
    LineRenderer_GetCounts(input, NULL, &trianglesCount, &minSpanLength);
    if (output.length < minSpanLength)
    {
        return false;
    }
    assert(trianglesCount % 2 == 0);
    if (input->segmented)
    {
        //( 0)-( 4)( 8)-(12)(16)-(20)(24)-(28)(32)-(36)
        //|   /   ||   /   ||   /   ||   /   ||   /   |
        //( 1)-( 5)( 9)-(13)(17)-(21)(25)-(29)(33)-(37)
        //|   /   ||   /   ||   /   ||   /   ||   /   |
        //( 2)-( 6)(10)-(14)(18)-(22)(26)-(30)(34)-(38)
        //|   /   ||   /   ||   /   ||   /   ||   /   |
        //( 3)-( 7)(11)-(15)(19)-(23)(27)-(31)(35)-(39)

        const u32 nextColumn = input->circumferenceVertices;
        const u32 nextRow = 1;
        const u32 pointsCount = input->maxPointsIsListCapacity ? (u32)input->points.capacity : (u32)input->points.count;
        const u32 segmentsCount = pointsCount - 1;

        u32 columnStart = 0;
        u32 columnPosition = 0;

        for (u32 i = 0; i < trianglesCount; i += 2)
        {
            const u32 triangleIndexStart = i * 6;
            //Triangle 1
            output[triangleIndexStart + 0] = i + columnStart;
            output[triangleIndexStart + 1] = i + columnStart + nextColumn;
            output[triangleIndexStart + 2] = i + columnStart + nextRow;
            //Triangle 2
            output[triangleIndexStart + 3] = i + columnStart + nextRow;
            output[triangleIndexStart + 4] = i + columnStart + nextColumn;
            output[triangleIndexStart + 5] = i + columnStart + nextColumn + nextRow;
            
            columnPosition++;
            if (columnPosition == input->circumferenceVertices - 1)
            {
                //columnStart += verticesPerColumn * 2
                columnStart += input->circumferenceVertices * 2;
                columnPosition = 0;
            }
        }
    }
    else
    {
        //00-04-08-12-16-20
        //|/ |/ |/ |/ |/ |
        //01-05-09-13-17-21
        //|/ |/ |/ |/ |/ |
        //02-06-10-14-18-22
        //|/ |/ |/ |/ |/ |
        //03-07-11-15-19-23
        const u32 nextColumn = input->circumferenceVertices;
        const u32 nextRow = 1;
        for (u32 i = 0; i < trianglesCount; i += 2)
        {
            const u32 triangleIndexStart = i * 6;
            //Triangle 1
            output[triangleIndexStart + 0] = i;
            output[triangleIndexStart + 1] = i + nextColumn;
            output[triangleIndexStart + 2] = i + nextRow;
            //Triangle 2
            output[triangleIndexStart + 3] = i + nextRow;
            output[triangleIndexStart + 4] = i + nextColumn;
            output[triangleIndexStart + 5] = i + nextColumn + nextRow;
        }
    }
    return true;
}