#pragma once
#include "Allocator.h"
#include "Astral.Base.h"
#include <string.h>

def_delegate(ListEqlFunc, bool, const void *, const void *);

typedef struct List
{
    IAllocator allocator;
    void *ptr;
    size_t capacity;
    size_t count;
    size_t itemSize;
} List;

inline List List_Empty()
{
    List list = {};
    return list;
}
inline List List_Create(IAllocator allocator, size_t itemSize)
{
    List list = {allocator, NULL, 0, 0, itemSize};
    return list;
}

inline void List_EnsureArrayCapacity(List *self, size_t minCapacity)
{
    if (self->capacity < minCapacity)
    {
        size_t newCapacity = self->capacity;
        if (newCapacity == 0)
        {
            newCapacity = 4;
        }
        while (newCapacity <= minCapacity)
        {
            newCapacity *= 2;
        }
        void *newPtr = IAllocator_Allocate(self->allocator, self->itemSize * newCapacity);
        if (self->ptr != NULL)
        {
            memcpy(newPtr, self->ptr, self->capacity * self->itemSize);
            IAllocator_Free(self->allocator, self->ptr);
        }
        self->ptr = newPtr;
        self->capacity = newCapacity;
    }
}
inline void List_EnsureArrayCapacityDefaulted(List *self, size_t minCapacity, const void *defaultItem)
{
    if (self->capacity < minCapacity)
    {
        size_t newCapacity = self->capacity;
        if (newCapacity == 0)
        {
            newCapacity = 4;
        }
        while (newCapacity <= minCapacity)
        {
            newCapacity *= 2;
        }
        void *newPtr = IAllocator_Allocate(self->allocator, self->itemSize * newCapacity);
        if (self->ptr != NULL)
        {
            memcpy(newPtr, self->ptr, self->capacity * self->itemSize);
            IAllocator_Free(self->allocator, self->ptr);
        }
        for (size_t i = self->capacity; i < newCapacity; i++)
        {
            memcpy((uint8_t *)newPtr + i * self->itemSize, defaultItem, self->itemSize);
        }
        self->ptr = newPtr;
        self->capacity = newCapacity;
    }
}
inline void List_Add(List *self, const void *item)
{
    List_EnsureArrayCapacity(self, self->count + 1);
    memcpy((uint8_t *)self->ptr + self->itemSize * self->count, item, self->itemSize);
    self->count++;
}
inline void List_AddEmpty(List *self)
{
    List_EnsureArrayCapacity(self, self->count + 1);
    self->count++;
}
inline int64_t List_IndexOf(const List *self, const void *item, ListEqlFunc eqlFunc)
{
    for (size_t i = 0; i < self->count; i++)
    {
        const void *checking = (const uint8_t *)self->ptr + i * self->itemSize;
        if (eqlFunc == NULL)
        {
            if (memcmp(checking, item, self->itemSize) == 0)
            {
                return (int64_t)i;
            }
        }
        else if (eqlFunc(checking, item))
        {
            return (int64_t)i;
        }
    }
    return -1;
}
inline void List_RemoveAtPullback(List *self, size_t atIndex)
{
    if (atIndex >= self->count)
    {
        return;
    }
    for (size_t i = atIndex; i < self->count - 1; i++)
    {
        memcpy((uint8_t *)self->ptr + self->itemSize * i, (uint8_t *)self->ptr + self->itemSize * (i + 1), self->itemSize);
    }
    self->count--;
}
inline void List_RemoveAtSwap(List *self, size_t atIndex)
{
    if (atIndex >= self->count)
    {
        return;
    }
    if (atIndex < (int64_t)self->count - 1)
    {
        memcpy((uint8_t *)self->ptr + self->itemSize * atIndex, (uint8_t *)self->ptr + self->itemSize * (self->count - 1), self->itemSize);
    }
    self->count--;
}
inline bool List_RemovePullback(List *self, const void *item, ListEqlFunc eqlFunc)
{
    int64_t index = List_IndexOf(self, item, eqlFunc);
    if (index == -1)
    {
        return false;
    }
    List_RemoveAtPullback(self, (size_t)index);
    return true;
}
inline bool List_RemoveSwap(List *self, const void *item, ListEqlFunc eqlFunc)
{
    int64_t index = List_IndexOf(self, item, eqlFunc);
    if (index == -1)
    {
        return false;
    }
    List_RemoveAtSwap(self, (size_t)index);
    return true;
}
inline bool List_InsertPushback(List *self, const void *item, int64_t atIndex)
{
    if (atIndex > self->count)
    {
        return false;
    }
    List_EnsureArrayCapacity(self, self->count + 1);

    for (int64_t i = (int64_t)self->count; i > atIndex; i--)
    {
        void *ptrI = (uint8_t *)self->ptr + self->itemSize * i;
        void *ptrIminus1 = (uint8_t *)self->ptr + self->itemSize * (i - 1);
        memcpy(ptrI, ptrIminus1, self->itemSize);
    }
    memcpy((uint8_t *)self->ptr + self->itemSize * atIndex, item, self->itemSize);
    self->count += 1;

    return true;
}
inline bool List_InsertSwap(List *self, const void *item, int64_t atIndex)
{
    if (atIndex > self->count)
    {
        return false;
    }
    List_EnsureArrayCapacity(self, self->count + 1);

    if (atIndex < self->count)
    {
        memcpy((uint8_t *)self->ptr + self->itemSize * (self->count - 1), (uint8_t *)self->ptr + self->itemSize * atIndex, self->itemSize);
    }
    memcpy((uint8_t *)self->ptr + self->itemSize * atIndex, item, self->itemSize);
    self->count += 1;

    return true;
}
inline void *List_InsertOverride(List *self, const void *item, int64_t atIndex)
{
    List_EnsureArrayCapacity(self, atIndex + 1);

    void *intoPos = (uint8_t *)self->ptr + self->itemSize * atIndex;
    if (item != NULL)
    {
        memcpy(intoPos, item, self->itemSize);
    }
    else
    {
        memset(intoPos, 0, self->itemSize);
    }
    if (self->count < atIndex + 1)
    {
        self->count = atIndex + 1;
    }
    return intoPos;
}
inline void *List_InsertOverride_ArrayDefaulted(List *self, const void *item, int64_t atIndex, const void *arrayDefaultItem)
{
    List_EnsureArrayCapacityDefaulted(self, atIndex + 1, arrayDefaultItem);

    void *intoPos = (uint8_t *)self->ptr + self->itemSize * atIndex;
    if (item != NULL)
    {
        memcpy(intoPos, item, self->itemSize);
    }
    else
    {
        memset(intoPos, 0, self->itemSize);
    }
    if (self->count < atIndex + 1)
    {
        self->count = atIndex + 1;
    }
    return intoPos;
}
inline void List_Clear(List *self)
{
    self->count = 0;
}
inline void *List_Get(List *self, size_t index)
{
    return (uint8_t *)self->ptr + index * self->itemSize;
}
inline void *List_Pop(List *self)
{
    void *result = (uint8_t *)self->ptr + (self->count - 1) * self->itemSize;
    self->count--;
    return result;
}
inline void List_Deinit(List *self)
{
    if (self->ptr != NULL)
    {
        IAllocator_Free(self->allocator, self->ptr);
        self->ptr = NULL;
    }
}

#define LIST(type, allocatorVar) List_Create(allocatorVar, sizeof(type))
#define LIST_GETREF(listVar, type, index) (type *)List_Get(listVar, index)
#define LIST_GET(listVar, type, index) *LIST_GETREF(listVar, type, index)