#pragma once
#include "./IndexedList.h"

typedef struct IntMap
{
    IndexedList dense;
    List sparse;
} IntMap;

inline IntMap IntMap_Empty()
{
    const IntMap result = {};
    return result;
}
inline IntMap IntMap_Create(IAllocator allocator, size_t itemSize)
{
    IntMap result;
    result.dense = IndexedList_Create(allocator, itemSize);
    result.sparse = List_Create(allocator, sizeof(uint32_t));
    return result;
}
inline void IntMap_Deinit(IntMap *self)
{
    IndexedList_Deinit(&self->dense);
    List_Deinit(&self->sparse);
}

inline uint32_t IntMap_Add(IntMap *self, uint32_t key, const void *value)
{
    const uint32_t defaultIndex = 0xffffffff;
    uint32_t index = IndexedList_Add(&self->dense, value);
    List_InsertOverride_ArrayDefaulted(&self->sparse, &index, (int64_t)key, &defaultIndex);
    return index;
}
inline bool IntMap_Contains(IntMap *self, uint32_t key)
{
    const uint32_t defaultIndex = 0xffffffff;
    return key < self->sparse.count && LIST_GET(&self->sparse, uint32_t, key) != defaultIndex;
}
inline void *IntMap_Get(IntMap *self, uint32_t key)
{
    const uint32_t defaultIndex = 0xffffffff;
    if (key >= self->sparse.count)
    {
        return NULL;
    }
    uint32_t index = LIST_GET(&self->sparse, uint32_t, key);
    if (index == defaultIndex)
    {
        return NULL;
    }

    return IndexedList_Get(&self->dense, (size_t)index);
}
inline void *IntMap_Remove(IntMap *self, uint32_t key)
{
    const uint32_t defaultIndex = 0xffffffff;
    if (key >= self->sparse.count)
    {
        return NULL;
    }
    uint32_t *index = LIST_GETREF(&self->sparse, uint32_t, key);
    if (*index == defaultIndex)
    {
        return NULL;
    }

    void *popped = IndexedList_RemoveAndReturn(&self->dense, (size_t)(*index));
    *index = defaultIndex;
    return popped;
}
inline void IntMap_Clear(IntMap *self)
{
    IndexedList_Clear(&self->dense);
    for (uint32_t i = 0; i < self->sparse.count; i++)
    {
        uint32_t *index = LIST_GETREF(&self->sparse, uint32_t, i);
        *index = 0xffffffff;
    }
    //List_Clear(&self->sparse);
}