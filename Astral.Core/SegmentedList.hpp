#pragma once

#include "Allocators.hpp"
#include <stdlib.h>
#include <assert.h>
#include "Array.hpp"
#include "option.hpp"

namespace collections
{
    template <typename T>
    struct SegmentedList
    {
        def_delegate(EqlFunc, bool, T, T);

        IAllocator allocator;
        T **pages;
        usize pageSize;
        usize pagesCount;

        usize count;

        inline SegmentedList()
        {
            allocator = IAllocator{};
            pages = NULL;
            pageSize = 0;
            pagesCount = 0;
            count = 0;
        }
        inline SegmentedList(IAllocator myAllocator)
        {
            allocator = myAllocator;
            pages = NULL;
            this->pageSize = 32;
            pagesCount = 0;
            count = 0;
        }
        inline SegmentedList(IAllocator myAllocator, usize pageSize)
        {
            allocator = myAllocator;
            pages = NULL;
            this->pageSize = pageSize;
            pagesCount = 0;
            count = 0;
        }
        inline void deinit()
        {
            if (pages != NULL && this->allocator.allocFunction != NULL)
            {
                for (u32 i = 0; i < pagesCount; i++)
                {
                    this->allocator.Free(pages[i]);
                }
                this->allocator.Free(pages);
            }
            pages = NULL;
            count = 0;
            pageSize = 0;
            pagesCount = 0;
            allocator = {};
        }

        inline usize EnsureArrayCapacity(usize minCapacity)
        {
            const usize totalCapacity = pageSize * pagesCount;
            if (totalCapacity < minCapacity)
            {
                usize newPageCount = (minCapacity / pageSize) + 1;
                T **newPagesPtr = (T **)allocator.Allocate(sizeof(T *) * newPageCount);

                if (pages != NULL)
                {
                    for (usize i = 0; i < pagesCount; i++)
                    {
                        newPagesPtr[i] = pages[i];
                    }
                    allocator.Free(pages);
                }
                for (usize i = pagesCount; i < newPageCount; i++)
                {
                    newPagesPtr[i] = (T *)allocator.Allocate(sizeof(T) * pageSize);
                }
                pages = newPagesPtr;
                pagesCount = newPageCount;
            }
            return count / pageSize;
        }
        inline T *Add(T item)
        {
            const usize toPage = EnsureArrayCapacity(count + 1);
            const usize inPageIndex = count % pageSize;
            pages[toPage][inPageIndex] = item;
            count += 1;

            return &pages[toPage][inPageIndex];
        }
        inline void Clear()
        {
            count = 0;
        }
        inline T *Get(usize index) const
        {
            assert(index < count);
            usize atPage = index / pageSize;
            return &pages[atPage][index % pageSize];
        }
        inline T& operator[](usize index) const
        {
            assert(index < count);
            usize atPage = index / pageSize;
            return pages[atPage][index % pageSize];
        }
        inline void RemoveAt_Swap(usize index)
        {
            assert(index < count);
            if (index < count - 1)
            {
                const usize indexPage = index / pageSize;
                const usize lastPage = pagesCount - 1;

                pages[indexPage][index % pageSize] = pages[lastPage][(count - 1) % pageSize];
            }
            count -= 1;
        }
        inline void RemoveAt_Pullback(usize index)
        {
            assert(index < count);

            if (index < count - 1)
            {
                const usize lastPage = pagesCount - 1;

                for (usize i = index + 1; i < count - 1; i++)
                {
                    const usize atPage = i / pageSize;
                    const usize nextElemPage = (i + 1) / pageSize;

                    pages[atPage][i % pageSize] = pages[nextElemPage][(i + 1) % pageSize];
                }
            }
            count -= 1;
        }
    };
}