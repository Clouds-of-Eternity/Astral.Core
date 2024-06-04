#pragma once

#include "Linxc.h"
#include "allocators.hpp"
#include <stdlib.h>
#include <assert.h>
#include "array.hpp"
#include "option.hpp"

namespace collections
{
    template <typename T>
    struct list
    {
        def_delegate(EqlFunc, bool, T, T);

        IAllocator allocator;
        T *ptr;
        usize capacity;
        usize count;

        list()
        {
            allocator = IAllocator{};
            ptr = NULL;
            capacity = 0;
            count = 0;
        }
        list(IAllocator myAllocator)
        {
            allocator = myAllocator;
            ptr = NULL;
            capacity = 0;
            count = 0;
        }
        list(IAllocator myAllocator, usize minCapacity)
        {
            this->allocator = myAllocator;
            ptr = (T*)this->allocator.Allocate(sizeof(T) * minCapacity);
            for (usize i = 0; i < minCapacity; i++)
            {
                ptr[i] = T();
            }
            capacity = minCapacity;
            count = 0;
        }
        void deinit()
        {
            if (ptr != NULL && this->allocator.allocFunction != NULL)
            {
                this->allocator.FREEPTR(ptr);
            }
            count = 0;
            capacity = 0;
        }

        void EnsureArrayCapacity(usize minCapacity)
        {
            if (capacity <= minCapacity)
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
                    for (usize i = 0; i < capacity; i += 1)
                    {
                        newPtr[i] = ptr[i];
                    }
                    allocator.Free(ptr);
                }
                for (usize i = capacity; i < newCapacity; i++)
                {
                    newPtr[i] = T();
                }
                ptr = newPtr;
                capacity = newCapacity;
            }
        }
        void Add(T item)
        {
            EnsureArrayCapacity(count + 1);
            ptr[count] = item;
            count += 1;
        }
        void Insert(T item, usize at)
        {
            EnsureArrayCapacity(count + 1);
            for (i64 i = (i64)count; i > (i64)at; i--)
            {
                ptr[i] = ptr[i - 1];
            }
            ptr[at] = item;
            count += 1;
        }
        void InsertAll(T* item, usize numItems, usize at)
        {
            EnsureArrayCapacity(count + numItems);
            for (i64 i = (i64)count -1 + numItems; i >= at + numItems; i--)
            {
                ptr[i] = ptr[i - 1];
            }
            for (i32 i = 0; i < numItems; i++)
            {
                ptr[at + i] = item[i];
            }
            count += numItems;
        }
        void Clear()
        {
            for (usize i = 0; i < count; i++)
            {
                ptr[i] = T();
            }
            count = 0;
        }
        T *Get(usize index)
        {
            return &ptr[index];
        }
        void RemoveAt_Swap(usize index)
        {
            assert(index >= 0 && index < count);
            //ptr[index].~();
            if (index < count - 1)
            {
                ptr[index] = ptr[count - 1];
            }
            ptr[count - 1] = T();
            count -= 1;
        }
        void RemoveAt_Pullback(usize index)
        {
            assert(index >= 0 && index < count);
            if (index < count - 1)
            {
                for (usize i = index; i < count - 1; i++)
                {
                    ptr[i] = ptr[i + 1];
                }
            }
            ptr[count - 1] = T();
            count -= 1;
        }
        void RemoveManyAt(usize index, usize numRemoves)
        {
            assert(index >= 0 && index + numRemoves <= count);
            for (usize i = index; i < count - numRemoves; i++)
            {
                ptr[i] = ptr[i + numRemoves];
            }
            for (usize i = count - numRemoves; i < count; i++)
            {
                ptr[i] = T();
            }
            count -= numRemoves;
        }
        collections::Array<T> ToClonedArray(IAllocator newAllocator)
        {
            if (this->ptr == NULL)
            {
                return collections::Array<T>();
            }
            T *slice = (T*)newAllocator.Allocate(sizeof(T) * this->count);
            for (usize i = 0; i < this->count; i++)
            {
                slice[i] = this->ptr[i];
            }
            collections::Array<T> result = collections::Array<T>(this->allocator, slice, this->count);
            return result;
        }
        collections::Array<T> ToOwnedArray()
        {
            if (this->ptr == NULL)
            {
                return collections::Array<T>();
            }
            T *slice = (T*)allocator.Allocate(sizeof(T) * this->count);
            for (usize i = 0; i < this->count; i++)
            {
                slice[i] = this->ptr[i];
            }
            collections::Array<T> result = collections::Array<T>(this->allocator, slice, this->count);
            deinit();
            return result;
        }
        collections::Array<T> ToOwnedArrayWith(IAllocator newAllocator)
        {
            if (this->ptr == NULL)
            {
                return collections::Array<T>(newAllocator);
            }
            T *slice = (T*)newAllocator.Allocate(sizeof(T) * this->count);
            for (usize i = 0; i < this->count; i++)
            {
                slice[i] = this->ptr[i];
            }
            collections::Array<T> result = collections::Array<T>(newAllocator, slice, this->count);
            deinit();
            return result;
        }
        option<usize> Contains(T value, EqlFunc eqlFunc)
        {
            for (usize i = 0; i < count; i++)
            {
                if (eqlFunc(this->ptr[i], value))
                {
                    option<usize>(i);
                }
            }
            return option<usize>();
        }
        collections::Array<T> ToRefArray()
        {
            return collections::Array<T>(NULL, this->ptr, this->count);
        }
        void AddAllDeinit(collections::list<T> *from)
        {
            for (usize i = 0; i < from->count; i++)
            {
                Add(from->ptr[i]);
            }
            //memcpy(this->ptr, from->ptr, from->count * sizeof(T));
            from->deinit();
        }
    };
}