#pragma once
#include "Linxc.h"
#include <stdlib.h>

def_delegate(allocFunc, void *, void *, usize);
def_delegate(freeFunc, void, void *, void *);

//these dont matter if it's inlined or not since we're indirectly calling them anyways
inline void* CAllocator_Allocate(void* instance, usize bytes)
{
    return malloc(bytes);
}
inline void CAllocator_Free(void* instance, void* ptr)
{
    free(ptr);
}

#define FREEPTR(ptr) FreeAndSetNull((void**)&ptr)

struct IAllocator
{
    void* instance;
    allocFunc allocFunction;
    freeFunc freeFunction;

    inline void *Allocate(usize bytes)
    {
        return allocFunction(instance, bytes);
    }
    inline void FreeAndSetNull(void **ptr)
    {
        freeFunction(instance, *ptr);
        *ptr = NULL;
    }
    inline void Free(void *ptr)
    {
        freeFunction(instance, ptr);
    }

    inline IAllocator()
    {
        this->instance = NULL;
        this->allocFunction = NULL;
        this->freeFunction = NULL;
    }
    inline IAllocator(void *instance, allocFunc AllocateFunc, freeFunc freeFunc)
    {
        this->instance = instance;
        this->allocFunction = AllocateFunc;
        this->freeFunction = freeFunc;
    }

    inline bool operator==(IAllocator other)
    {
        return other.allocFunction == allocFunction && other.freeFunction == freeFunction && other.instance == instance;
    }
    inline bool operator!=(IAllocator other)
    {
        return other.allocFunction != allocFunction || other.freeFunction != freeFunction || other.instance != instance;
    }
};

inline IAllocator GetCAllocator()
{
    return IAllocator(NULL, &CAllocator_Allocate, &CAllocator_Free);
}