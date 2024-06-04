#pragma once
#include "vector.hpp"
#include "allocators.hpp"

inline void *ArenaAllocator_Allocate(void *instance, usize bytes);
inline void ArenaAllocator_Free(void *instance, void *ptr);

struct ArenaAllocator
{
    collections::vector<void*> ptrs;
    IAllocator baseAllocator;

    inline ArenaAllocator()
    {
        ptrs = collections::vector<void *>();
        baseAllocator = IAllocator();
    }
    inline ArenaAllocator(IAllocator base)
    {
        this->ptrs = collections::vector<void*>(base);
        this->baseAllocator = base;
    }
    inline IAllocator AsAllocator()
    {
        return IAllocator(this, &ArenaAllocator_Allocate, &ArenaAllocator_Free);
    }

    inline void Clear()
    {
        for (usize i = 0; i < this->ptrs.count; i++)
        {
            this->baseAllocator.Free(*this->ptrs.Get(i));
        }
        this->ptrs.Clear();
    }
    inline void deinit()
    {
        for (usize i = 0; i < this->ptrs.count; i++)
        {
            this->baseAllocator.Free(*this->ptrs.Get(i));
        }
        ptrs.deinit();
    }
};

void* ArenaAllocator_Allocate(void* instance, usize bytes)
{
    ArenaAllocator* self = (ArenaAllocator*)instance;
    void* result = self->baseAllocator.Allocate(bytes);
    self->ptrs.Add(result);
    return result;
}
void ArenaAllocator_Free(void* instance, void* ptr)
{

}