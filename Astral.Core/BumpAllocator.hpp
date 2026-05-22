#pragma once
#include "Allocators.hpp"

struct BumpAllocatorInternals
{
    IAllocator baseAllocator;
    usize totalSize;
    usize currentOffset;
};
inline void *BumpAllocator_Allocate(void *instance, size_t bytes);
inline void BumpAllocator_Free(void *instance, void *ptr);

struct BumpAllocator
{
    void *payload;

    inline BumpAllocatorInternals *GetInternals()
    {
        return (BumpAllocatorInternals *)this->payload;
    }
    inline void *GetAllocation()
    {
        return (u8 *)this->payload + sizeof(BumpAllocatorInternals);
    }
    inline IAllocator AsAllocator()
    {
        return IAllocator(payload, &BumpAllocator_Allocate, &BumpAllocator_Free);
    }

    inline BumpAllocator()
    {
        payload = NULL;
    }
    inline BumpAllocator(void *payload)
    {
        this->payload = payload;
    }
    inline BumpAllocator(IAllocator baseAllocator, usize totalRequiredSize)
    {
        payload = baseAllocator.Allocate(sizeof(BumpAllocatorInternals) + totalRequiredSize);
        BumpAllocatorInternals *internals = GetInternals();
        internals->baseAllocator = baseAllocator;
        internals->currentOffset = 0;
        internals->totalSize = totalRequiredSize;
    }
    inline void deinit()
    {
        if (payload != NULL)
        {
            IAllocator baseAllocator = GetInternals()->baseAllocator;
            baseAllocator.Free(payload);
        }
    }
};

void *BumpAllocator_Allocate(void *instance, size_t bytes)
{
    BumpAllocator allocator = {instance};
    BumpAllocatorInternals *internals = allocator.GetInternals();

    if (internals->currentOffset + bytes > internals->totalSize)
    {
        return NULL;
    }

    void *result = (u8 *)allocator.GetAllocation() + internals->currentOffset;
    internals->currentOffset += bytes;
    return result;
}
void BumpAllocator_Free(void *instance, void *ptr)
{
    //does nothing
}