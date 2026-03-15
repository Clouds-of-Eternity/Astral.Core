#pragma once
#include "List.hpp"
#include "Allocators.hpp"

inline void *ArenaAllocator_Allocate(void *instance, usize bytes);
inline void ArenaAllocator_Free(void *instance, void *ptr);

struct ArenaAllocator
{
    collections::List<void*> *ptrs;
    IAllocator baseAllocator;

    inline ArenaAllocator()
    {
        ptrs = NULL; //(collections::List<void *> *)baseAllocator.Allocate(sizeof(collections::List<void *>)); // collections::List<void *>();
        //*ptrs = collections::List<void *>();
        baseAllocator = IAllocator();
    }
    inline ArenaAllocator(IAllocator base)
    {
        ptrs = (collections::List<void *> *)base.Allocate(sizeof(collections::List<void *>)); // collections::List<void *>();
        *ptrs = collections::List<void *>(base);
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
    collections::List<void *> *self = (collections::List<void *> *)instance;
    void* result = self->allocator.Allocate(bytes);
    self->Add(result);
    return result;
}
void ArenaAllocator_Free(void* instance, void* ptr)
{

}