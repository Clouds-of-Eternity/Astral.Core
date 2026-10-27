#pragma once
#include "List.hpp"
#include "Span.hpp"
#include "Maths/Quaternion.hpp"
#include "Maths/Util.hpp"

struct LineTriangulationVertexArray
{
    void * ptr;

    usize length;
    usize structSize;

    u16 positionVectorFieldOffset;
    u16 positionVectorSize;

    u16 uvFieldOffset;
    u16 uvSize;

    u16 directionFieldOffset;
    u16 directionSize;

    inline void SetPosition(u32 index, Maths::Vec3 position)
    {
        if (index >= length || positionVectorSize == 0)
        {
            return;
        }
        void *ptrAtIndex = (u8*)ptr + index * structSize + positionVectorFieldOffset;
        if (positionVectorSize == 8)
        {
            *(Maths::Vec2 *)ptrAtIndex = position.ToVector2();
        }
        else if (positionVectorSize >= 12)
        {
            *(Maths::Vec3 *)ptrAtIndex = position;
        }
    }
    inline void SetUV(u32 index, Maths::Vec2 UV)
    {
        if (index >= length || uvSize == 0)
        {
            return;
        }
        void *ptrAtIndex = (u8*)ptr + index * structSize + uvFieldOffset;
        if (uvSize == 8)
        {
            *(Maths::Vec2 *)ptrAtIndex = UV;
        }
        else if (uvSize == 4)
        {
            //store as 2 u16s
            u16 x = UV.X * AC_U16Max;
            u16 y = UV.Y * AC_U16Max;
            *(u32 *)ptrAtIndex = (x << 16) | y;
        }
    }
    inline void SetDirection(u32 index, Maths::Quaternion direction)
    {
        if (index >= length || directionSize == 0)
        {
            return;
        }
        void *ptrAtIndex = (u8*)ptr + index * structSize + directionFieldOffset;

        if (directionSize == 16)
        {
            *(Maths::Quaternion *)ptrAtIndex = direction;
        }
        //store as 4 bytes
        else if (directionSize == 4)
        {
            i8 bytesX = (i8)(direction.X * AC_I8Max);
            i8 bytesY = (i8)(direction.Y * AC_I8Max);
            i8 bytesZ = (i8)(direction.Z * AC_I8Max);
            i8 bytesW = (i8)(direction.W * AC_I8Max);

            *(u32 *)ptrAtIndex = (bytesX << 24) | (bytesY << 16) | (bytesZ << 8) | (bytesW);
        }
    }
    // inline void *GetAt(u32 index) const
    // {
    //     if (index >= length)
    //     {
    //         return NULL;
    //     }
    //     return (u8 *)ptr + structSize * index;
    // }
};

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

    inline LineTriangulationInput()
    {
        points = collections::List<Maths::Vec3>();
        maxPointsIsListCapacity = false;
        segmented = false;
        circumferenceVertices = 0;
    }
    inline LineTriangulationInput(IAllocator allocator)
    {
        points = collections::List<Maths::Vec3>(allocator);
        maxPointsIsListCapacity = false;
        segmented = false;
        circumferenceVertices = 2;
    }
    inline LineTriangulationInput(IAllocator allocator, u32 circumferenceVertices, bool segmented, bool maxPointsIsListCapacity)
    {
        points = collections::List<Maths::Vec3>(allocator);
        this->maxPointsIsListCapacity = maxPointsIsListCapacity;
        this->segmented = segmented;
        this->circumferenceVertices = circumferenceVertices;
    }
    inline void deinit()
    {
        points.deinit();
    }
};

inline void LineTriangulator_GetCounts(const LineTriangulationInput *self, u32 *outVerticesCount, u32 *outTrianglesCount, u32 *outIndicesCount)
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
inline bool LineTriangulator_MapVertices(const LineTriangulationInput *input, LineTriangulationVertexArray output, Maths::Vec2 uvMult, float lineWidthMult, bool isFlat)
{
    u32 trianglesCount;
    u32 minSpanLength = 0;
    LineTriangulator_GetCounts(input, &minSpanLength, &trianglesCount, NULL);
    if (output.length < minSpanLength)
    {
        return false;
    }

    //pre-calculate circumference offsets.
    //In these arrays, 'X' will refer to the Y offset, while 'Y' will refer to the Z offset.
    Maths::Vec2 circumferenceOffsetSpanStack[8];
    Maths::Vec2 *circumferenceOffsets = NULL;
    if (input->circumferenceVertices > 8)
    {
        circumferenceOffsets = (Maths::Vec2 *)DEFAULT_ALLOC(sizeof(Maths::Vec2) * input->circumferenceVertices);
    }
    else
    {
        circumferenceOffsets = circumferenceOffsetSpanStack;
    }
    for (u32 i = 0; i < input->circumferenceVertices; i++)
    {
        if (isFlat)
        {
            circumferenceOffsets[i] = Maths::Vec2((i / (float)(input->circumferenceVertices - 1)) * lineWidthMult, 0.0f);
        }
        else
        {
            const float angle = i * (360.0f / input->circumferenceVertices) * Degree2Radian;
            const float s = sinf(angle);
            const float c = cosf(angle);
            circumferenceOffsets[i] = Maths::Vec2(c * lineWidthMult, s * lineWidthMult);
        }
    }

    if (input->segmented)
    {
        //( 0)-( 4)( 8)-(12)(16)-(20)(24)-(28)(32)-(36)
        //|   /   ||   /   ||   /   ||   /   ||   /   |
        //( 1)-( 5)( 9)-(13)(17)-(21)(25)-(29)(33)-(37)
        //|   /   ||   /   ||   /   ||   /   ||   /   |
        //( 2)-( 6)(10)-(14)(18)-(22)(26)-(30)(34)-(38)
        //|   /   ||   /   ||   /   ||   /   ||   /   |
        //( 3)-( 7)(11)-(15)(19)-(23)(27)-(31)(35)-(39)

        const u32 segmentsCount = input->points.count - 1;
        for (u32 i = 0; i < segmentsCount; i++)
        {
            const Maths::Quaternion segmentDir = Maths::Quaternion::FromAxisAngle((input->points[i + 1] - input->points[i]).Normalized(), 0.0f);

            const Maths::Vec3 point = input->points[i];
            const Maths::Vec3 next = input->points[i + 1];
            for (u32 j = 0; j < input->circumferenceVertices; j++)
            {
                const u32 firstColumnIndex = (i * 2) * input->circumferenceVertices + j;
                const u32 nextColumnIndex = (i * 2 + 1) * input->circumferenceVertices + j;

                Maths::Vec3 pointOffset = Maths::Vec3(0.0f, circumferenceOffsets[j].X, circumferenceOffsets[j].Y);
                Maths::Vec3 nextOffset = Maths::Vec3(0.0f, circumferenceOffsets[j].X, circumferenceOffsets[j].Y);

                pointOffset = segmentDir.Transform(pointOffset);
                nextOffset = segmentDir.Transform(nextOffset);

                output.SetPosition(firstColumnIndex, point + pointOffset);
                output.SetUV(firstColumnIndex, Maths::Vec2(i / (float)(input->points.count - 1), j / (float)(input->circumferenceVertices - 1)) * uvMult);
                output.SetDirection(firstColumnIndex, segmentDir);

                output.SetPosition(nextColumnIndex, next + nextOffset);
                output.SetUV(nextColumnIndex, Maths::Vec2((i + 1) / (float)(input->points.count - 1), j / (float)(input->circumferenceVertices - 1)) * uvMult);
                output.SetDirection(nextColumnIndex, segmentDir); //TODO: apply interpolation.
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
        Maths::Quaternion dir = {};

        for (u32 i = 0; i < input->points.count; i++)
        {
            if (i < input->points.count - 1)
            {
                if (isFlat)
                {
                    dir = Maths::Quaternion::FromAxisAngle(Maths::Vec3(0.0f, 0.0f, 1.0f), Maths::DirectionTowards(input->points[i].ToVector2(), input->points[i + 1].ToVector2()));
                }
                else dir = Maths::Quaternion::FromAxisAngle((input->points[i + 1] - input->points[i]).Normalized(), 0.0f);
            }
            const Maths::Vec3 point = input->points[i];

            for (u32 j = 0; j < input->circumferenceVertices; j++)
            {
                Maths::Vec3 pointOffset = Maths::Vec3(0.0f, circumferenceOffsets[j].X, circumferenceOffsets[j].Y);
                pointOffset = dir.Transform(pointOffset);

                const u32 toIndex = i * input->circumferenceVertices + j;
                output.SetPosition(toIndex, point + pointOffset);
                output.SetUV(toIndex, Maths::Vec2(i / (float)(input->points.count - 1), j / (float)(input->circumferenceVertices - 1)) * uvMult);
                output.SetDirection(toIndex, dir);
            }
        }
    }

    if (input->circumferenceVertices > 8)
    {
        DEFAULT_FREE(circumferenceOffsets);
    }
    return true;
}
inline bool LineTriangulator_MapIndices(const LineTriangulationInput *input, Span<u32> output, u32 firstVertex)
{
    u32 trianglesCount;
    u32 minSpanLength = 0;
    LineTriangulator_GetCounts(input, NULL, &trianglesCount, &minSpanLength);
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

        u32 triangleIndexStart = 0;
        for (u32 i = 0; i < input->points.count - 1; i++)
        {
            for (u32 j = 0; j < input->circumferenceVertices - 1; j++)
            {
                const u32 index = i * input->circumferenceVertices * 2 + j + firstVertex;
                
                //Triangle 1
                assert(index - firstVertex + nextColumn < minSpanLength);
                output[triangleIndexStart + 0] = index;
                output[triangleIndexStart + 1] = index + nextColumn;
                output[triangleIndexStart + 2] = index + nextRow;
                //Triangle 2
                output[triangleIndexStart + 3] = index + nextRow;
                output[triangleIndexStart + 4] = index + nextColumn;
                output[triangleIndexStart + 5] = index + nextColumn + nextRow;
                
                triangleIndexStart += 6;
            }
            columnStart += input->circumferenceVertices * 2;
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
        u32 triangleIndexStart = 0;
        for (u32 i = 0; i < input->points.count - 1; i++)
        {
            for (u32 j = 0; j < input->circumferenceVertices - 1; j++)
            {
                const u32 index = i * input->circumferenceVertices + j + firstVertex;
                output[triangleIndexStart + 0] = index;
                output[triangleIndexStart + 1] = index + nextColumn;
                output[triangleIndexStart + 2] = index + nextRow;
                //Triangle 2
                output[triangleIndexStart + 3] = index + nextRow;
                output[triangleIndexStart + 4] = index + nextColumn;
                output[triangleIndexStart + 5] = index + nextColumn + nextRow;
                triangleIndexStart += 6;
            }
        }
        // for (u32 i = 0; i < trianglesCount; i += 2)
        // {
        //     const u32 triangleIndexStart = i * 3;
        //     //Triangle 1
        //     output[triangleIndexStart + 0] = i;
        //     output[triangleIndexStart + 1] = i + nextColumn;
        //     output[triangleIndexStart + 2] = i + nextRow;
        //     //Triangle 2
        //     output[triangleIndexStart + 3] = i + nextRow;
        //     output[triangleIndexStart + 4] = i + nextColumn;
        //     output[triangleIndexStart + 5] = i + nextColumn + nextRow;
        // }
    }
    return true;
}