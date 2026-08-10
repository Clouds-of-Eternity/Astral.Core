#pragma once
#include "List.h"

typedef struct ArenaAllocator
{
    List *ptrs;
} ArenaAllocator;

static inline ArenaAllocator ArenaAllocator_Empty()
{
    ArenaAllocator result = {NULL};
    return result;
}
static inline ArenaAllocator ArenaAllocator_Create(IAllocator baseAllocator)
{
    List *listPtr = IAllocator_Allocate(baseAllocator, sizeof(List));
    *listPtr = List_Create(baseAllocator, sizeof(void *));
    ArenaAllocator result = {listPtr};

    return result;
}
static inline void ArenaAllocator_Deinit(ArenaAllocator *self)
{
    if (self->ptrs != NULL)
    {
        IAllocator allocator = self->ptrs->allocator;
        List_Deinit(self->ptrs);
        IAllocator_Free(allocator, self->ptrs);
        self->ptrs = NULL;
    }
}

static inline void* ArenaAllocator_Allocate(void* instance, size_t bytes)
{
    List *ptrs = (List *)instance;

    void* result = IAllocator_Allocate(ptrs->allocator, bytes);
    List_Add(ptrs, &result);
    return result;
}
static inline void ArenaAllocator_Free(void* instance, void* ptr)
{
    //do nothing
}
static inline IAllocator ArenaAllocator_AsAllocator(const ArenaAllocator *self)
{
    IAllocator result = {self->ptrs, &ArenaAllocator_Allocate, &ArenaAllocator_Free};

    return result;
}