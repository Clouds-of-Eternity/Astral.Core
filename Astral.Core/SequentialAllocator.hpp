#pragma once
#include "allocators.hpp"

inline void *SequentialAllocator_Allocate(void *instance, usize bytes);
inline void SequentialAllocator_Free(void *instance, void *ptr);

struct SequentialAllocatorInternals
{
    IAllocator baseAllocator;
    usize totalSize;
    usize currentOffset;
};
struct SequentialAllocator
{
    void *payload;

    inline SequentialAllocatorInternals& GetInternals()
    {
        return ((SequentialAllocatorInternals *)payload)[0];
    }
    inline void *GetAllocation()
    {
        return payload + sizeof(SequentialAllocatorInternals);
    }
    inline SequentialAllocator()
    {
        payload = NULL;
    }
    inline SequentialAllocator(void *payload)
    {
        this->payload = payload;
    }
    inline SequentialAllocator(IAllocator baseAllocator, usize totalSize)
    {
        payload = baseAllocator.Allocate(sizeof(SequentialAllocatorInternals) + totalSize);
        SequentialAllocatorInternals &internals = GetInternals();
        internals.currentOffset = 0;
        internals.totalSize = totalSize;
        internals.baseAllocator = baseAllocator;
    }
    inline void deinit()
    {
        if (payload != NULL)
        {
            IAllocator baseAllocator = GetInternals().baseAllocator;
            baseAllocator.Free(payload);
        }
    }
    inline IAllocator AsAllocator()
    {
        return IAllocator(payload, &SequentialAllocator_Allocate, &SequentialAllocator_Free);
    }
};

void *SequentialAllocator_Allocate(void *instance, usize bytes)
{
    SequentialAllocator allocator = SequentialAllocator(instance);
    SequentialAllocatorInternals &internals = allocator.GetInternals();

    if (internals.currentOffset + bytes > internals.totalSize)
    {
        return NULL;
    }

    void *result = allocator.GetAllocation() + internals.currentOffset;
    internals.currentOffset += bytes;
    return result;
}
void SequentialAllocator_Free(void *instance, void *ptr)
{
    //does nothing
}