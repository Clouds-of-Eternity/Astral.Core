#pragma once
#include "List.hpp"

namespace collections
{
    template <typename T>
    struct IndexedList
    {
        collections::List<T> list;
        collections::List<u32> freeIndices;

        inline IndexedList()
        {
            list = collections::List<T>();
            freeIndices = collections::List<u32>();
        }
        inline IndexedList(IAllocator allocator)
        {
            list = collections::List<T>(allocator);
            freeIndices = collections::List<u32>(allocator);
        }

        inline T *Get(usize index)
        {
            return &list.ptr[index];
        }
        inline T& operator[](usize index)
        {
            return list.ptr[index];
        }

        inline u32 Add(T item)
        {
            if (freeIndices.count == 0)
            {
                list.Add(item);
                return (u32)list.count - 1;
            }
            u32 intoIndex = freeIndices.Pop();
            list[intoIndex] = item;
            return intoIndex;
        }
        inline u32 AddDefault()
        {
            if (freeIndices.count == 0)
            {
                list.Add({});
                return (u32)list.count - 1;
            }
            u32 intoIndex = freeIndices.Pop();
            return intoIndex;
        }
        inline void Remove(u32 index)
        {
            if (index < list.count)
            {
                freeIndices.Add(index);
            }
        }
        inline void RemoveAndDeinit(u32 index)
        {
            if (index < list.count)
            {
                list.Get(index)->deinit();
                list[index] = {};
                freeIndices.Add(index);
            }
        }
        inline void deinit()
        {
            list.deinit();
            freeIndices.deinit();
        }
    };
}