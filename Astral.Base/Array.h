#pragma once
#include "Allocator.h"
#include <string.h>

typedef struct Array
{
    IAllocator allocator;
    void *ptr;
    size_t itemSize;
    size_t length;
} Array;

static inline Array Array_Empty()
{
    const Array result = {};
    return result;
}
static inline Array Array_Create(IAllocator allocator, size_t itemSize, size_t length)
{
    Array result;
    result.allocator = allocator;
    result.itemSize = itemSize;
    result.length = length;
    result.ptr = NULL;

    if (length > 0)
    {
        result.ptr = IAllocator_Allocate(allocator, itemSize * length);
    }
    return result;
}
static inline Array Array_CreateDefaultInitted(IAllocator allocator, size_t itemSize, size_t length)
{
    Array result;
    result.allocator = allocator;
    result.itemSize = itemSize;
    result.length = length;
    result.ptr = NULL;

    if (length > 0)
    {
        result.ptr = IAllocator_Allocate(allocator, itemSize * length);
        memset(result.ptr, 0, itemSize * length);
    }
    return result;
}
static inline Array Array_CreateFromExisting(IAllocator allocator, void *existingPtr, size_t itemSize, size_t length)
{
    const Array result = {allocator, existingPtr, itemSize, length};
    return result;
}
static inline Array Array_CreateAsContainer(void *existingPtr, size_t itemSize, size_t length)
{
    const IAllocator emptyAllocator = {};
    const Array result = {emptyAllocator, existingPtr, itemSize, length};
    return result;
}

static inline void *Array_Get(Array *self, size_t index)
{
    return (uint8_t *)self->ptr + index * self->itemSize;
}
static inline void Array_Deinit(Array *self)
{
    if (self->ptr != NULL)
    {
        IAllocator_Free(self->allocator, self->ptr);
        self->ptr = NULL;
    }
}

#define ARRAY(type, allocatorVar, length) Array_Create(allocatorVar, sizeof(type), length)
#define ARRAY_GETREF(arrayVar, type, index) (type *)Array_Get(arrayVar, index)
#define ARRAY_GET(arrayVar, type, index) *ARRAY_GETREF(arrayVar, type, index)
#define ARRAY_AS(arrayVar, type) ((type *)arrayVar.ptr)