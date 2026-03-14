#include "./StringPool.h"
#include <threads.h>

thread_local StringPool globalStringPool;

StringPool *GetGlobalStringPool()
{
    if (globalStringPool.buffers.ptr == NULL)
    {
        globalStringPool = StringPool_Create(GetCAllocator());
    }
    return &globalStringPool;
}