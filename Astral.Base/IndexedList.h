#pragma once
#include "./List.h"

/// @brief An IndexedList is a collection of items that also tracks the indices that have been
/// removed, such that when a new item is added, it will instead occupy an already existing but
/// vacant slot before it attempts to append to the list.
typedef struct IndexedList
{
    List list;
    List freeIndices;
} IndexedList;

inline IndexedList IndexedList_Empty()
{
    const IndexedList result = {};
    return result;
}
inline IndexedList IndexedList_Create(IAllocator allocator, size_t itemSize)
{
    IndexedList result;
    result.list = List_Create(allocator, itemSize);
    result.freeIndices = List_Create(allocator, sizeof(uint32_t));
    return result;
}
inline void *IndexedList_Get(IndexedList *self, size_t index)
{
    return (uint8_t *)self->list.ptr + index * self->list.itemSize;
}
inline uint32_t IndexedList_Add(IndexedList *self, const void *item)
{
    if (self->freeIndices.count == 0)
    {
        List_Add(&self->list, item);
        return (uint32_t)self->list.count - 1;
    }
    uint32_t intoIndex = *(uint32_t*)List_Pop(&self->freeIndices);
    void *slot = List_Get(&self->list, intoIndex);
    memcpy(slot, item, self->list.itemSize);
    return intoIndex;
}
inline uint32_t IndexedList_AddDefaultZeroInitialized(IndexedList *self)
{
    if (self->freeIndices.count == 0)
    {
        List_AddEmpty(&self->list);
        return (uint32_t)self->list.count - 1;
    }
    uint32_t intoIndex = *(uint32_t*)List_Pop(&self->freeIndices);
    void *slot = List_Get(&self->list, intoIndex);
    memset(slot, 0, self->list.itemSize);
    return intoIndex;
}
inline uint32_t IndexedList_AddDefault(IndexedList *self, bool *created)
{
    if (self->freeIndices.count == 0)
    {
        *created = true;
        List_AddEmpty(&self->list);
        return (uint32_t)self->list.count - 1;
    }
    *created = false;
    uint32_t intoIndex = *(uint32_t*)List_Pop(&self->freeIndices);
    void *slot = List_Get(&self->list, intoIndex);
    return intoIndex;
}
inline void IndexedList_Remove(IndexedList *self, uint32_t index)
{
    if (index < self->list.count)
    {
        List_Add(&self->freeIndices, &index);
    }
}
inline void *IndexedList_RemoveAndReturn(IndexedList *self, uint32_t index)
{
    if (index < self->list.count)
    {
        void *result = IndexedList_Get(self, index);
        List_Add(&self->freeIndices, &index);

        return result;
    }
    return NULL;
}
inline void IndexedList_Deinit(IndexedList *self)
{
    List_Deinit(&self->list);
    List_Deinit(&self->freeIndices);
}