#pragma once
#include "DenseSet.hpp"
#include "List.hpp"

namespace collections
{
    template <typename T>
    struct SparseSet
    {
        IAllocator allocator;

        collections::DenseSet<i64> IDtoActualIndex;
        collections::List<T> storage;
        collections::List<i64> emptySpaces;

        inline SparseSet()
        {
            allocator = {};
            IDtoActualIndex = collections::DenseSet<i64>();
            storage = collections::List<T>();
            emptySpaces = collections::List<i64>();
        }
        inline SparseSet(IAllocator alloc)
        {
            allocator = alloc;
            IDtoActualIndex = collections::DenseSet<i64>(alloc, -1);
            storage = collections::List<T>(alloc);
            emptySpaces = collections::List<i64>(alloc);
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