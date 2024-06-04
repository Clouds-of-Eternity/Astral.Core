#pragma once
#include "Linxc.h"
#include "allocators.hpp"
#include "stdio.h"

namespace collections
{
    template <typename T>
    struct denseset
    {
        IAllocator allocator;
        T *ptr;
        usize capacity;

        denseset()
        {
            allocator = IAllocator{};
            ptr = NULL;
            capacity = 0;
        }
        denseset(IAllocator myAllocator)
        {
            allocator = myAllocator;
            ptr = NULL;
            capacity = 0;
        }
        denseset(IAllocator myAllocator, usize minCapacity)
        {
            this->allocator = myAllocator;
            ptr = NULL;
            capacity = 0;
            //capacity = minCapacity;
            EnsureArrayCapacity(minCapacity);
        }
        void deinit()
        {
            if (ptr != NULL && this->allocator.allocFunction != NULL)
            {
                this->allocator.FREEPTR(ptr);
            }
            capacity = 0;
        }
        void EnsureArrayCapacity(usize minCapacity)
        {
            if (capacity <= minCapacity || ptr == NULL)
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
                if (ptr != NULL)
                {
                    for (usize i = 0; i < capacity; i++)
                    {
                        newPtr[i] = ptr[i];
                    }
                }
                else
                {
                    for (usize i = 0; i < capacity; i++)
                    {
                        newPtr[i] = T();
                    }
                }
                for (usize i = capacity; i < newCapacity; i++)
                {
                    newPtr[i] = T();
                }
                if (ptr != NULL)
                {
                    allocator.Free(ptr);
                }
                ptr = newPtr;
                capacity = newCapacity;
            }
        }
        T* Insert(usize index, T value)
        {
            EnsureArrayCapacity(index);
            ptr[index] = value;
            return &ptr[index];
        }
        T *Get(usize index)
        {
            if (index >= capacity)
            {
                return NULL;
            }
            T *result = &ptr[index];
            return result;
        }
    };
}
