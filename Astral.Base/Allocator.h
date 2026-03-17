#pragma once
#include "Astral.Base.h"

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

#ifndef NO_LIBC

#include <stdlib.h>
inline void* CAllocator_Allocate(void* instance, size_t bytes)
{
    return malloc(bytes);
}
inline void CAllocator_Free(void* instance, void* ptr)
{
    free(ptr);
}

inline IAllocator GetCAllocator()
{
    IAllocator result = {NULL, &CAllocator_Allocate, &CAllocator_Free};
}
#endif


inline IAllocator IAllocator_Make(void *instance, allocFunc allocateFunc, freeFunc freeFunc)
{
    IAllocator result = {instance, allocateFunc, freeFunc};
    return result;
}
inline void *IAllocator_Allocate(IAllocator self, size_t bytes)
{
    return self.allocFunction(self.instance, bytes);
}
inline void IAllocator_Free(IAllocator self, void *memory)
{
    self.freeFunction(self.instance, memory);
}