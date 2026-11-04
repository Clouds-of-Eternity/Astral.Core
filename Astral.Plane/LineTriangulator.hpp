#pragma once
#include "List.hpp"
#include "Span.hpp"
#include "Maths/Quaternion.hpp"
#include "Maths/Util.hpp"

def_delegate(LineTriangulationSetMiscDataFunc, void, void *verticesPtr, void *userData, u32 vertexIndex, float coveredDistance, u32 segmentIndex, u32 segmentsCount);

struct LineTriangulationVertexArray
{
    void *ptr;
    void *userData;

    u32 length;
    u32 structSize;

    u16 positionVectorFieldOffset;
    u16 positionVectorSize;

    u16 uvFieldOffset;
    u16 uvSize;

    u16 directionFieldOffset;
    u16 directionSize;

    u16 lineWidthFieldOffset;
    u16 lineWidthSize;

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
    inline void SetLineWidth(u32 index, float width)
    {
        if (index >= length || lineWidthSize == 0)
        {
            return;
        }
        void *ptrAtIndex = (u8*)ptr + index * structSize + lineWidthFieldOffset;
        if (lineWidthSize == 4)
        {
            *(float *)ptrAtIndex = width;
        }
        else if (lineWidthSize == 1)
        {
            *(u8 *)ptrAtIndex = (u8)width;
        }
    }
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

    inline void GetCounts(u32 *_Nullable outVerticesCount, u32 *_Nullable outTrianglesCount, u32 *_Nullable outIndicesCount) const
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

        const u32 maxPoints = this->maxPointsIsListCapacity ? (u32)this->points.capacity : (u32)this->points.count;

        const u32 segmentsCount = maxPoints - 1;
        if (outVerticesCount != NULL)
        {
            if (!this->segmented)
            {
                *outVerticesCount = maxPoints * this->circumferenceVertices;
            }
            else
            {
                *outVerticesCount = segmentsCount * this->circumferenceVertices * 2;
            }
        }
        const u32 totalTriangles = segmentsCount * (this->circumferenceVertices - 1) * 2;
        if (outTrianglesCount != NULL)
        {
            *outTrianglesCount = totalTriangles;
        }
        if (outIndicesCount != NULL)
        {
            *outIndicesCount = totalTriangles * 3;
        }
    }
    inline bool MapVertices(LineTriangulationVertexArray output, Maths::Vec2 uvMult, float lineWidthMult, bool isFlat, bool uvIsDistanceInWorldSpace, LineTriangulationSetMiscDataFunc _Nullable setMiscDataCallback, u32 *_Nullable outputActualUsedIndices) const
    {
        u32 trianglesCount;
        u32 minSpanLength = 0;
        GetCounts(&minSpanLength, &trianglesCount, NULL);
        if (output.length < minSpanLength)
        {
            return false;
        }

        i32 usedSegments = 0;

        //pre-calculate circumference offsets.
        //In these arrays, 'X' will refer to the Y offset, while 'Y' will refer to the Z offset.
        Maths::Vec2 circumferenceOffsetSpanStack[8];
        Maths::Vec2 *circumferenceOffsets = NULL;
        if (this->circumferenceVertices > 8)
        {
            circumferenceOffsets = (Maths::Vec2 *)DEFAULT_ALLOC(sizeof(Maths::Vec2) * this->circumferenceVertices);
        }
        else
        {
            circumferenceOffsets = circumferenceOffsetSpanStack;
        }

        const bool hasDedicatedLineWidthSlot = output.lineWidthSize > 0;
        if (!hasDedicatedLineWidthSlot)
        {
            //bake line width offsets into core positions
            for (u32 i = 0; i < this->circumferenceVertices; i++)
            {
                if (isFlat)
                {
                    circumferenceOffsets[i] = Maths::Vec2((i / (float)(this->circumferenceVertices - 1)) * lineWidthMult, 0.0f);
                }
                else
                {
                    const float angle = i * (360.0f / this->circumferenceVertices) * Degree2Radian;
                    const float s = sinf(angle);
                    const float c = cosf(angle);
                    circumferenceOffsets[i] = Maths::Vec2(c * lineWidthMult, s * lineWidthMult);
                }
            }
        }

        if (this->segmented)
        {
            //( 0)-( 4)( 8)-(12)(16)-(20)(24)-(28)(32)-(36)
            //|   /   ||   /   ||   /   ||   /   ||   /   |
            //( 1)-( 5)( 9)-(13)(17)-(21)(25)-(29)(33)-(37)
            //|   /   ||   /   ||   /   ||   /   ||   /   |
            //( 2)-( 6)(10)-(14)(18)-(22)(26)-(30)(34)-(38)
            //|   /   ||   /   ||   /   ||   /   ||   /   |
            //( 3)-( 7)(11)-(15)(19)-(23)(27)-(31)(35)-(39)
            
            float coveredDistance = 0.0f;
            const u32 segmentsCount = this->points.count - 1;
            for (u32 i = 0; i < segmentsCount; i++)
            {
                const Maths::Vec3 curr = this->points[i];
                const Maths::Vec3 next = this->points[i + 1];
                if (Maths::Vec3::AlmostEqual(curr, next))
                {
                    continue;
                }
                const float currToNextDist = Maths::Vec3::Distance(curr, next);
                const Maths::Quaternion segmentDir = Maths::Quaternion::FromAxisAngle((next - curr) / currToNextDist, 0.0f);

                for (u32 j = 0; j < this->circumferenceVertices; j++)
                {
                    const u32 firstColumnIndex = (usedSegments * 2) * this->circumferenceVertices + j;
                    const u32 nextColumnIndex = (usedSegments * 2 + 1) * this->circumferenceVertices + j;

                    Maths::Vec3 pointOffset = Maths::Vec3();
                    Maths::Vec3 nextOffset = Maths::Vec3();

                    if (!hasDedicatedLineWidthSlot)
                    {
                        pointOffset = Maths::Vec3(0.0f, circumferenceOffsets[j].X, circumferenceOffsets[j].Y);
                        nextOffset = Maths::Vec3(0.0f, circumferenceOffsets[j].X, circumferenceOffsets[j].Y);

                        pointOffset = segmentDir.Transform(pointOffset);
                        nextOffset = segmentDir.Transform(nextOffset);
                    }
                    else
                    {
                        output.SetLineWidth(firstColumnIndex, lineWidthMult);
                        output.SetLineWidth(nextColumnIndex, lineWidthMult);
                    }

                    const Maths::Quaternion segmentDirMultByCV = segmentDir * (float)this->circumferenceVertices;

                    output.SetPosition(firstColumnIndex, curr + pointOffset);
                    output.SetUV(firstColumnIndex, Maths::Vec2(uvIsDistanceInWorldSpace ? coveredDistance : (usedSegments / (float)segmentsCount), j / (float)(this->circumferenceVertices - 1)) * uvMult);
                    output.SetDirection(firstColumnIndex, segmentDirMultByCV);

                    output.SetPosition(nextColumnIndex, next + nextOffset);
                    output.SetUV(nextColumnIndex, Maths::Vec2(uvIsDistanceInWorldSpace ? coveredDistance + currToNextDist : ((usedSegments + 1) / (float)segmentsCount), j / (float)(this->circumferenceVertices - 1)) * uvMult);
                    output.SetDirection(nextColumnIndex, segmentDirMultByCV);

                    if (setMiscDataCallback != NULL)
                    {
                        setMiscDataCallback(output.ptr, output.userData, firstColumnIndex, coveredDistance, usedSegments, segmentsCount);
                        setMiscDataCallback(output.ptr, output.userData, nextColumnIndex, coveredDistance + currToNextDist, usedSegments + 1, segmentsCount);
                    }
                }
                coveredDistance += currToNextDist;
                usedSegments++;
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
            float coveredDistance = 0.0f;
            Maths::Quaternion dir = {};

            for (u32 i = 0; i < this->points.count; i++)
            {
                const Maths::Vec3 curr = this->points[i];
                float currToNextDist = 0.0f;
                if (i < this->points.count - 1)
                {
                    const Maths::Vec3 next = this->points[i + 1];
                    if (Maths::Vec3::AlmostEqual(curr, next))
                    {
                        continue;
                    }

                    currToNextDist = Maths::Vec3::Distance(curr, next);
                    if (isFlat)
                    {
                        dir = Maths::Quaternion::FromAxisAngle(Maths::Vec3(0.0f, 0.0f, 1.0f), Maths::DirectionTowards(curr.ToVector2(), next.ToVector2()));
                    }
                    else dir = Maths::Quaternion::FromAxisAngle((next - curr) / currToNextDist, 0.0f);
                }

                for (u32 j = 0; j < this->circumferenceVertices; j++)
                {
                    const u32 toIndex = usedSegments * this->circumferenceVertices + j;
                    Maths::Vec3 pointOffset = Maths::Vec3();
                    if (!hasDedicatedLineWidthSlot)
                    {
                        pointOffset = Maths::Vec3(0.0f, circumferenceOffsets[j].X, circumferenceOffsets[j].Y);
                        pointOffset = dir.Transform(pointOffset);
                    }
                    else
                    {
                        output.SetLineWidth(toIndex, lineWidthMult);
                    }

                    output.SetPosition(toIndex, curr + pointOffset);
                    output.SetUV(toIndex, Maths::Vec2(uvIsDistanceInWorldSpace ? coveredDistance : (usedSegments / (float)(this->points.count - 1)), j / (float)(this->circumferenceVertices - 1)) * uvMult);
                    output.SetDirection(toIndex, dir * (float)circumferenceVertices);

                    if (setMiscDataCallback != NULL)
                    {
                        setMiscDataCallback(output.ptr, output.userData, toIndex, coveredDistance, usedSegments, this->points.count - 1);
                    }
                }
                coveredDistance += currToNextDist;
                usedSegments++;
            }
        }

        if (this->circumferenceVertices > 8)
        {
            DEFAULT_FREE(circumferenceOffsets);
        }

        if (outputActualUsedIndices != NULL)
        {
            *outputActualUsedIndices = (usedSegments - 1) * (this->circumferenceVertices - 1) * 2 * 3;
        }
        return true;
    }
    inline bool MapIndices(Span<u32> output, u32 firstVertex) const
    {
        u32 trianglesCount;
        u32 minSpanLength = 0;
        GetCounts(NULL, &trianglesCount, &minSpanLength);
        if (output.length < minSpanLength)
        {
            return false;
        }
        assert(trianglesCount % 2 == 0);
        if (this->segmented)
        {
            //( 0)-( 4)( 8)-(12)(16)-(20)(24)-(28)(32)-(36)
            //|   /   ||   /   ||   /   ||   /   ||   /   |
            //( 1)-( 5)( 9)-(13)(17)-(21)(25)-(29)(33)-(37)
            //|   /   ||   /   ||   /   ||   /   ||   /   |
            //( 2)-( 6)(10)-(14)(18)-(22)(26)-(30)(34)-(38)
            //|   /   ||   /   ||   /   ||   /   ||   /   |
            //( 3)-( 7)(11)-(15)(19)-(23)(27)-(31)(35)-(39)

            const u32 nextColumn = this->circumferenceVertices;
            const u32 nextRow = 1;
            const u32 pointsCount = this->maxPointsIsListCapacity ? (u32)this->points.capacity : (u32)this->points.count;
            const u32 segmentsCount = pointsCount - 1;

            u32 columnStart = 0;

            u32 triangleIndexStart = 0;
            for (u32 i = 0; i < segmentsCount; i++)
            {
                for (u32 j = 0; j < this->circumferenceVertices - 1; j++)
                {
                    const u32 index = i * this->circumferenceVertices * 2 + j + firstVertex;
                    
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
                columnStart += this->circumferenceVertices * 2;
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
            const u32 segmentsCount = this->points.count - 1;
            const u32 nextColumn = this->circumferenceVertices;
            const u32 nextRow = 1;
            u32 triangleIndexStart = 0;
            for (u32 i = 0; i < segmentsCount; i++)
            {
                for (u32 j = 0; j < this->circumferenceVertices - 1; j++)
                {
                    const u32 index = i * this->circumferenceVertices + j + firstVertex;
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
};