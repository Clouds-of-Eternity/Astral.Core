#pragma once
#include "Linxc.h"
#include "allocators.hpp"

namespace collections
{
    template<typename T>
    struct queue
    {
        IAllocator allocator;
        T *items;
        usize firstItemIndex;
        usize lastItemIndex;
        usize count;
        usize capacity;

        queue()
        {
            allocator = IAllocator{};
            items = NULL;
            firstItemIndex = 0;
            lastItemIndex = 0;
            count = 0;
            capacity = 0;
        }
        queue(IAllocator allocator)
        {
            this->allocator = allocator;
            items = NULL;
            firstItemIndex = 0;
            lastItemIndex = 0;
            count = 0;
            capacity = 0;
        }

        void deinit()
        {
            allocator.Free(items);
            items = NULL;
            firstItemIndex = 0;
            lastItemIndex = 0;
            count = 0;
            capacity = 0;
            allocator = IAllocator{};
        }
        void EnsureArrayCapacity(usize minCapacity)
        {
            if (capacity < minCapacity)
            {
                usize newCapacity = capacity;
                if (newCapacity == 0)
                {
                    newCapacity = 4;
                }
                while (newCapacity <= minCapacity)
                {
                    newCapacity *= 2;
                }
                T *newPtr = (T*)allocator.Allocate(sizeof(T) * newCapacity);
                if (items != NULL)
                {
                    if (firstItemIndex < lastItemIndex) {
                        memcpy(newPtr, items + firstItemIndex, sizeof(T) * count);
                        //Array.Copy(_array, _head, newarray, 0, _size);
                    } else {
                        memcpy(newPtr, items + firstItemIndex, sizeof(T) * (capacity - firstItemIndex));
                        memcpy(newPtr + (capacity - firstItemIndex), items, sizeof(T) * lastItemIndex);
                    }
                }
                allocator.FREEPTR(items);
                items = newPtr;
                capacity = newCapacity;
                firstItemIndex = 0;
                lastItemIndex = (count == newCapacity) ? 0 : count; 
            }
        }

        T* Enqueue(T item)
        {
            EnsureArrayCapacity(count + 1);

            items[lastItemIndex] = item;
            T *result = &items[lastItemIndex];
            lastItemIndex = (lastItemIndex + 1) % capacity;

            count++;
            return result;
        }
        T Dequeue()
        {
            if (count == 0)
            {
                return T();
            }
            T removed = items[firstItemIndex];
            items[firstItemIndex] = T();
            firstItemIndex = (firstItemIndex + 1) % capacity;
            count--;
            return removed;
        }
        T *Peek()
        {
            if (count == 0)
            {
                return NULL;
            }
            return &items[firstItemIndex];
        }
        void Clear()
        {
            count = 0;
            firstItemIndex = 0;
            lastItemIndex = 0;
        }
    };
}