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
        usize pageCapacity;
        usize pagesCount;

        usize count;

        inline SegmentedList()
        {
            allocator = IAllocator{};
            pages = NULL;
            capacity = 0;
            count = 0;
        }
        inline SegmentedList(IAllocator myAllocator, usize pageCapacity)
        {
            allocator = myAllocator;
            pages = NULL;
            this->pageCapacity = pageCapacity;
            pagesCount = 0;
            count = 0;
        }
        void deinit()
        {
            if (pages != NULL && this->allocator.allocFunction != NULL)
            {
                for (u32 i = 0; i < pagesCount; i++)
                {
                    this->allocator.Free(pages[i]);
                }
                this->allocator.Free(pages);
            }
            count = 0;
            pageCapacity = 0;
            pagesCount = 0;
        }

        usize EnsureArrayCapacity(usize minCapacity)
        {
            const usize totalCapacity = pageCapacity * pagesCount;
            if (totalCapacity < minCapacity)
            {
                usize newPageCount = (minCapacity / pageCapacity) + 1;
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
                    newPagesPtr[i] = (T *)allocator.Allocate(sizeof(T) * pageCapacity);
                }
                pages = newPagesPtr;
                pagesCount = newPageCount;
            }
            return count / pageCapacity;
        }
        T *Add(T item)
        {
            const usize toPage = EnsureArrayCapacity(count + 1);
            const usize inPageIndex = count % pageCapacity;
            pages[toPage][inPageIndex] = item;
            count += 1;

            return &pages[toPage][inPageIndex];
        }
        void Clear()
        {
            count = 0;
        }
        T *Get(usize index) const
        {
            usize atPage = index / pageCapacity;
            return &pages[atPage][index % pageCapacity];
        }
        inline T& operator[](usize index) const
        {
            return pages[atPage][index % pageCapacity];
        }
        void RemoveAt_Swap(usize index)
        {
            assert(index >= 0 && index < count);
            if (index < count - 1)
            {
                const usize indexPage = index / pageCapacity;
                const usize lastPage = pagesCount - 1;

                pages[indexPage][index % pageCapacity] = pages[lastPage][(count - 1) % pageCapacity];
            }
            count -= 1;
        }
        void RemoveAt_Pullback(usize index)
        {
            assert(index >= 0 && index < count);

            if (index < count - 1)
            {
                const usize lastPage = pagesCount - 1;

                for (usize i = index + 1; i < count - 1; i++)
                {
                    const usize atPage = i / pageCapacity;
                    const usize nextElemPage = (i + 1) / pageCapacity;

                    pages[atPage][i % pageCapacity] = pages[nextElemPage][(i + 1) % pageCapacity];
                }
            }
            count -= 1;
        }
    };
}