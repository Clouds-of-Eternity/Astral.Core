#pragma once
#include "Linxc.h"
#include "Allocators.hpp"
#include "stdio.h"

namespace collections
{
    template <typename T>
    struct DenseSet
    {
        IAllocator allocator;
        T *ptr;
        T defaultValue;
        usize capacity;

        inline DenseSet()
        {
            allocator = IAllocator{};
            ptr = NULL;
            defaultValue = T{};
            capacity = 0;
        }
        inline DenseSet(IAllocator myAllocator)
        {
            allocator = myAllocator;
            ptr = NULL;
            defaultValue = T{};
            capacity = 0;
        }
        inline DenseSet(IAllocator myAllocator, T defaultValue)
        {
            allocator = myAllocator;
            ptr = NULL;
            this->defaultValue = defaultValue;
            capacity = 0;
        }
        inline T& operator[](usize index)
        {
            return ptr[index];
        }
        inline void deinit()
        {
            if (ptr != NULL && this->allocator.allocFunction != NULL)
            {
                this->allocator.FREEPTR(ptr);
            }
            capacity = 0;
        }
        inline void EnsureArrayCapacity(usize minCapacity)
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
                        newPtr[i] = defaultValue;
                    }
                }
                for (usize i = capacity; i < newCapacity; i++)
                {
                    newPtr[i] = defaultValue;
                }
                if (ptr != NULL)
                {
                    allocator.Free(ptr);
                }
                ptr = newPtr;
                capacity = newCapacity;
            }
        }
        inline T* Insert(usize index, T value)
        {
            EnsureArrayCapacity(index);
            ptr[index] = value;
            return &ptr[index];
        }
        inline T *Get(usize index)
        {
            if (index >= capacity)
            {
                return NULL;
            }
            T *result = &ptr[index];
            return result;
        }
        inline T GetCopyOr(usize index, T valueOnNotFound)
        {
            if (index >= capacity)
            {
                return valueOnNotFound;
            }
            return ptr[index];
        }
        inline void Remove(u32 index)
        {
            if (index <= capacity)
            {
                ptr[index] = T();
            }
        }
    };
}
