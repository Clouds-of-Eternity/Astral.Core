#pragma once
#include "Linxc.h"
#include <stdlib.h>

#define KiB_SIZE 1024
#define MiB_SIZE (KiB_SIZE * 1024)

def_delegate(allocFunc, void *, void *, size_t);
def_delegate(freeFunc, void, void *, void *);

typedef struct IAllocator
{
    void* instance;
    allocFunc allocFunction;
    freeFunc freeFunction;
} IAllocator;

static inline void* CAllocator_Allocate(void* instance, size_t bytes)
{
    return malloc(bytes);
}
static inline void CAllocator_Free(void* instance, void* ptr)
{
    free(ptr);
}

static inline IAllocator GetCAllocator()
{
    IAllocator result = {NULL, &CAllocator_Allocate, &CAllocator_Free};
    return result;
}


static inline IAllocator IAllocator_Make(void *instance, allocFunc allocateFunc, freeFunc freeFunc)
{
    IAllocator result = {instance, allocateFunc, freeFunc};
    return result;
}
static inline void *IAllocator_Allocate(IAllocator self, size_t bytes)
{
    return self.allocFunction(self.instance, bytes);
}
static inline void IAllocator_Free(IAllocator self, void *memory)
{
    self.freeFunction(self.instance, memory);
}