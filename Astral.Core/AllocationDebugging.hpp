#pragma once
#include <stdlib.h>

#ifdef ALLOCATION_DEBUGGING

void DebuggingLogAllocation(void *ptr, usize bytes);
void DebuggingLogFree(void *ptr);

inline void *DebuggingDefaultAlloc(usize bytes)
{
    void *result = malloc(bytes);
    DebuggingLogAllocation(result, bytes);
    return result;
    // atomic_fetch_add(&AllocatorDebugging::totalPointersAllocated, 1);
    // return malloc(bytes);
}
inline void DebuggingDefaultFree(void *ptr)
{
    DebuggingLogFree(ptr);
    free(ptr);
    // usize oldValue = atomic_fetch_sub(&AllocatorDebugging::totalPointersAllocated, 1);
    // assert(oldValue > 0);
    // return free(ptr);
}

//This essentially exists to intercept malloc() and free() calls in the program
#define DEFAULT_ALLOC(bytes) DebuggingDefaultAlloc(bytes)
#define DEFAULT_FREE(ptr) DebuggingDefaultFree(ptr)
#endif