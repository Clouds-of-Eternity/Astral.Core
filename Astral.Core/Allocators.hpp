#pragma once
#include "Linxc.h"
#include "AllocationDebugging.hpp"

#define KiB_SIZE 1024
#define MiB_SIZE (KiB_SIZE * 1024)
#ifndef DEFAULT_ALLOC
#define DEFAULT_ALLOC(bytes) malloc(bytes)
#endif
#ifndef DEFAULT_FREE
#define DEFAULT_FREE(ptr) free(ptr)
#endif

def_delegate(allocFunc, void *, void *, usize);
def_delegate(freeFunc, void, void *, void *);

//these dont matter if it's inlined or not since we're indirectly calling them anyways
inline void* CAllocator_Allocate(void* instance, usize bytes)
{
    return DEFAULT_ALLOC(bytes);
}
inline void CAllocator_Free(void* instance, void* ptr)
{
    DEFAULT_FREE(ptr);
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
    template <typename T>
    inline T *AllocateInstanceOf()
    {
        return (T *)Allocate(sizeof(T));
    }
    template <typename T>
    inline T *AllocateInstancesOf(usize num)
    {
        return (T *)Allocate(sizeof(T) * num);
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