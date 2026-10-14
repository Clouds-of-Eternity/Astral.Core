#pragma once
#include "Allocators.hpp"
#include <assert.h>
#include <string.h>

template <typename T>
struct SegmentedDenseSetArray
{
    T *array;
    usize index;
};

template <typename T>
struct SegmentedDenseSet
{
    IAllocator allocator;
    SegmentedDenseSetArray<T> *internalArray;

    usize count;
    usize poolSize;
    usize maxIndex;
    usize capacity;

    inline SegmentedDenseSet()
    {
        allocator = {};
        internalArray = NULL;
        count = 0;
        poolSize = 0;
        maxIndex = 0;
        capacity = 0;
    }
    inline SegmentedDenseSet(IAllocator allocator)
    {
        this->allocator = allocator;
        internalArray = NULL;
        count = 0;
        poolSize = 32;
        maxIndex = 0;
        capacity = 0;

        EnsureCapacity(1);
    }
    inline SegmentedDenseSet(IAllocator allocator, usize poolSize)
    {
        this->allocator = allocator;
        internalArray = NULL;
        count = 0;
        this->poolSize = poolSize;
        maxIndex = 0;
        capacity = 0;

        EnsureCapacity(1);
    }

    inline T& operator[](usize index) const
    {
        assert(index < maxIndex);

        usize poolIndex = index / poolSize;
        usize indexInPool = index % poolSize;
        return internalArray[poolIndex].array[indexInPool];
    }
    inline T *Get(usize index) const
    {
        if (index >= maxIndex)
        {
            return NULL;
        }

        usize poolIndex = index / poolSize;
        usize indexInPool = index % poolSize;
        return &internalArray[poolIndex].array[indexInPool];
    }
    inline T GetCopyOr(usize index, T valueOnNotFound) const
    {
        if (index >= maxIndex)
        {
            return valueOnNotFound;
        }

        usize poolIndex = index / poolSize;
        usize indexInPool = index % poolSize;
        return internalArray[poolIndex].array[indexInPool];
    }

    inline void EnsureCapacity(usize minPoolsCapacity)
    {
        if (internalArray == NULL || capacity < minPoolsCapacity)
        {
            usize prevCapacity = capacity;
            if (internalArray != NULL)
            {
                SegmentedDenseSetArray<T> *newInternalArray = (SegmentedDenseSetArray<T> *)allocator.Allocate(sizeof(SegmentedDenseSetArray<T>) * minPoolsCapacity);
                for (usize i = 0; i < capacity; i++)
                {
                    newInternalArray[i] = internalArray[i];
                }
                capacity = minPoolsCapacity;

                allocator.Free(internalArray);
                internalArray = newInternalArray;
            }
            else
            {
                internalArray = (SegmentedDenseSetArray<T> *)allocator.Allocate(sizeof(SegmentedDenseSetArray<T>) * minPoolsCapacity);
                capacity = minPoolsCapacity;
            }
            
            for (usize i = prevCapacity; i < capacity; i++)
            {
                internalArray[i].array = (T *)allocator.Allocate(sizeof(T) * poolSize);
                internalArray[i].index = i;
            }
        }
    }

    void Insert(usize atIndex, T item)
    {
        const usize poolIndex = atIndex / poolSize;
        const usize indexInPool = atIndex % poolSize;
        if (poolIndex + 1 >= capacity)
        {
            EnsureCapacity(poolIndex + 1);
        }
        
        SegmentedDenseSetArray<T> &pool = internalArray[poolIndex];

        pool.array[indexInPool] = item;
        if (atIndex + 1 > maxIndex)
        {
            maxIndex = atIndex + 1;
        }
        count += 1;
    }
    inline void ClearSet()
    {
        for (usize i = 0; i < capacity; i++)
        {
            memset(internalArray[i].array, NULL, poolSize * sizeof(T));
            //Array.Clear(internalArray[i].array, 0, (int)poolSize);
        }
        maxIndex = 0;
        count = 0;
    }
    inline void ClearSetDeinit()
    {
        for (usize i = 0; i < capacity; i++)
        {
            for (usize j = 0; j < poolSize; j++)
            {
                internalArray[i].array[j].deinit();
            }
        }
        maxIndex = 0;
        count = 0;
    }
    inline void ResetCount()
    {
        maxIndex = 0;
        count = 0;
    }

    inline void CopyTo(T *output, usize startIndex)
    {
        usize startPoolIndex = startIndex / poolSize;
        usize startInPoolIndex = startIndex % poolSize;

        usize copied = startIndex;
        for (usize i = startPoolIndex; i < capacity; i++)
        {
            for (usize j = startInPoolIndex; j < poolSize; j++)
            {
                output[copied] = internalArray[i].array[j];
                copied++;
                if (copied >= maxIndex)
                {
                    return;
                }
            }
        }
    }

    inline void deinit()
    {
        for (u32 i = 0; i < capacity; i++)
        {
            allocator.Free(internalArray[i].array);
            internalArray[i].array = NULL;
        }
        allocator.Free(internalArray);
        internalArray = NULL;

        allocator = {};
        count = 0;
        poolSize = 0;
        maxIndex = 0;
        capacity = 0;
    }
};