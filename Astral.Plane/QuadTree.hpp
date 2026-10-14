//Under construction!
/*#pragma once
#include "HashMap.hpp"
#include "Maths/Vec2.hpp"

template<typename T>
struct QuadTreeChunk
{
    Maths::Point2 position;
    i32 depth;

    union
    {
        QuadTreeChunk<T> *nextChunks;
        collections::List<T> values;
    }

    inline QuadTreeChunk()
    {
        position = Maths::Point2(0);
        depth = -1;
    }
    inline QuadTreeChunk(IAllocator allocator, Maths::Point2 chunkPosition, i32 depth, u32 maxDepth)
    {
        position = chunkPosition;
        this->depth = depth;
        if (depth < maxDepth)
        {
            nextChunks = (QuadTreeChunk<T> *)allocator.Allocate(sizeof(QuadTreeChunk<T>) * 4);
        }
    }
};
template<typename T>
struct QuadTree
{
    u32 maxDepth;
    collections::HashMap<Maths::Point2, QuadTreeChunk<T>> allChunks;

    inline void Insert()
    {

    }
};*/