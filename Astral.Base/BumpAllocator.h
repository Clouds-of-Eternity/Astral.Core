#pragma once
#include "./Allocator.h"

static inline void *BumpAllocator_Allocate(void *instance, size_t bytes);
static inline void BumpAllocator_Free(void *instance, void *ptr);

typedef struct BumpAllocatorInternals
{
    IAllocator baseAllocator;
    size_t totalSize;
    size_t currentOffset;
} BumpAllocatorInternals;

typedef struct BumpAllocator
{
    void *payload;
} BumpAllocator;

static inline BumpAllocatorInternals *BumpAllocator_GetInternals(BumpAllocator *self)
{
    return (BumpAllocatorInternals *)self->payload;
}
static inline void *BumpAllocator_GetAllocation(BumpAllocator *self)
{
    return (uint8_t *)self->payload + sizeof(BumpAllocatorInternals);
}

static inline BumpAllocator BumpAllocator_Empty()
{
    const BumpAllocator result = {};
    return result;
}
static inline BumpAllocator BumpAllocator_Create(IAllocator baseAllocator, size_t totalRequiredSize)
{
    BumpAllocator result;
    result.payload = IAllocator_Allocate(baseAllocator, sizeof(BumpAllocatorInternals) + totalRequiredSize);
    BumpAllocatorInternals *internals = BumpAllocator_GetInternals(&result);
    internals->baseAllocator = baseAllocator;
    internals->currentOffset = 0;
    internals->totalSize = totalRequiredSize;
    return result;
}
static inline void BumpAllocator_Deinit(BumpAllocator *self)
{
    if (self->payload != NULL)
    {
        IAllocator baseAllocator = BumpAllocator_GetInternals(self)->baseAllocator;
        IAllocator_Free(baseAllocator, self->payload);
    }
}
static inline IAllocator BumpAllocator_AsAllocator(BumpAllocator *self)
{
    const IAllocator result = {self->payload, &BumpAllocator_Allocate, &BumpAllocator_Free};
    return result;
}

void *BumpAllocator_Allocate(void *instance, size_t bytes)
{
    BumpAllocator allocator = {instance};
    BumpAllocatorInternals *internals = BumpAllocator_GetInternals(&allocator);

    if (internals->currentOffset + bytes > internals->totalSize)
    {
        return NULL;
    }

    void *result = (uint8_t *)BumpAllocator_GetAllocation(&allocator) + internals->currentOffset;
    internals->currentOffset += bytes;
    return result;
}
void BumpAllocator_Free(void *instance, void *ptr)
{
    //does nothing
}