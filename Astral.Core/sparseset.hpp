#pragma once
#include "denseset.hpp"
#include "vector.hpp"

namespace collections
{
    template <typename T>
    struct sparseset
    {
        IAllocator allocator;

        collections::denseset<i64> IDtoActualIndex;
        collections::vector<T> storage;
        collections::vector<i64> emptySpaces;

        inline sparseset()
        {
            allocator = {};
            IDtoActualIndex = collections::denseset<i64>();
            storage = collections::vector<T>();
            emptySpaces = collections::vector<i64>();
        }
        inline sparseset(IAllocator alloc)
        {
            allocator = alloc;
            IDtoActualIndex = collections::denseset<i64>(alloc, -1);
            storage = collections::vector<T>(alloc);
            emptySpaces = collections::vector<i64>(alloc);
        }
        inline void deinit()
        {
            IDtoActualIndex.deinit();
            storage.deinit();
            emptySpaces.deinit();
        }
        inline void Remove(usize ID)
        {
            emptySpaces.Add(*IDtoActualIndex.Get(ID));
            *IDtoActualIndex.Get(ID) = -1;
        }
        inline void Clear()
        {
            emptySpaces.Clear();
            storage.Clear();
            for (usize i = 0; i < IDtoActualIndex.capacity; i++)
            {
                IDtoActualIndex.ptr[i] = -1;
            }
        }
        inline T *Get(usize ID)
        {
            i64 index = IDtoActualIndex.GetCopyOr(ID, -1);
            if (index == -1)
            {
                return NULL;
            }
            return storage.Get(index);
        }
        inline T GetCopyOr(usize ID, T valueOnNotFound)
        {
            i64 index = IDtoActualIndex.GetCopyOr(ID, -1);
            if (index == -1)
            {
                return valueOnNotFound;
            }
            return *storage.Get(index);
        }
        inline void Insert(usize ID, T value)
        {
            if (emptySpaces.count > 0)
            {
                i64 index = emptySpaces[emptySpaces.count - 1];
                emptySpaces.RemoveAt_Swap(emptySpaces.count - 1);
                storage[index] = value;
                IDtoActualIndex.Insert(ID, index);
            }
            else
            {
                i64 newIndex = storage.count;
                storage.Add(value);
                IDtoActualIndex.Insert(ID, newIndex);
            }
        }
    };
}