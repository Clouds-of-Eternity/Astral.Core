#pragma once
#include "vector.hpp"
#include "allocators.hpp"

inline void *ArenaAllocator_Allocate(void *instance, usize bytes);
inline void ArenaAllocator_Free(void *instance, void *ptr);

struct ArenaAllocator
{
    collections::vector<void*> *ptrs;
    IAllocator baseAllocator;

    inline ArenaAllocator()
    {
        ptrs = NULL; //(collections::vector<void *> *)baseAllocator.Allocate(sizeof(collections::vector<void *>)); // collections::vector<void *>();
        //*ptrs = collections::vector<void *>();
        baseAllocator = IAllocator();
    }
    inline ArenaAllocator(IAllocator base)
    {
        ptrs = (collections::vector<void *> *)base.Allocate(sizeof(collections::vector<void *>)); // collections::vector<void *>();
        *ptrs = collections::vector<void *>(base);
        this->baseAllocator = base;
    }
    inline IAllocator AsAllocator()
    {
        return IAllocator(ptrs, &ArenaAllocator_Allocate, &ArenaAllocator_Free);
    }

    inline void Clear()
    {
        for (usize i = 0; i < this->ptrs->count; i++)
        {
            this->baseAllocator.Free(*this->ptrs->Get(i));
        }
        this->ptrs->Clear();
    }
    inline void deinit()
    {
        for (usize i = 0; i < this->ptrs->count; i++)
        {
            this->baseAllocator.Free(*this->ptrs->Get(i));
        }
        ptrs->deinit();
        baseAllocator.Free(ptrs);
    }
};

void* ArenaAllocator_Allocate(void* instance, usize bytes)
{
    collections::vector<void *> *self = (collections::vector<void *> *)instance;
    void* result = self->allocator.Allocate(bytes);
    self->Add(result);
    return result;
}
void ArenaAllocator_Free(void* instance, void* ptr)
{

}