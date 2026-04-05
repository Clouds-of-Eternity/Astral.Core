#pragma once
#include "Allocators.hpp"
#include <assert.h>

inline void *StackAllocator_Allocate(void *instance, usize bytes);
inline void StackAllocator_Free(void *instance, void *ptr);

struct StackPageHeader;

struct StackAllocatorHeader
{
    IAllocator baseAllocator;
    usize head;
    usize pageSize;
    StackPageHeader *currPage;
};
struct StackPageHeader
{
    StackPageHeader *nextStack;
};
struct StackAllocator
{
    void *payload;

    inline StackAllocator()
    {
        payload = NULL;
    }
    inline StackAllocator(void *payload)
    {
        this->payload = payload;
    }
    inline StackAllocator(IAllocator baseAllocator, usize pageSize)
    {
        payload = baseAllocator.Allocate(sizeof(StackAllocatorHeader) + sizeof(StackPageHeader) + pageSize);
        StackAllocatorHeader *internals = GetInternals();
        internals->baseAllocator = baseAllocator;
        internals->head = 0;
        internals->pageSize = pageSize;
        internals->currPage = GetPage(0);
        internals->currPage->nextStack = NULL;
    }

    inline StackAllocatorHeader *GetInternals()
    {
        return (StackAllocatorHeader *)payload;
    }
    inline StackPageHeader *GetPage(u32 index)
    {
        StackPageHeader *result = (StackPageHeader *)((u8 *)payload + sizeof(StackAllocatorHeader));
        while (index > 0 && result != NULL)
        {
            result = result->nextStack;
            index--;
        }
        return result;
    }
    inline void *GetPageAlloc(u32 index)
    {
        return (u8*)GetPage(index) + sizeof(StackPageHeader);
    }

    inline void deinit()
    {
        if (payload == NULL)
        {
            return;
        }
        IAllocator baseAllocator = ((StackAllocatorHeader *)payload)->baseAllocator;

        //dont need to free first pageHeader as that is under payload itself
        StackPageHeader *pageHeader = (StackPageHeader *)((u8*)payload + sizeof(StackAllocatorHeader));
        pageHeader = pageHeader->nextStack;

        while (pageHeader != NULL)
        {
            StackPageHeader *old = pageHeader;
            pageHeader = pageHeader->nextStack;
            baseAllocator.Free(old);
        }
        baseAllocator.Free(payload);
    }
    inline usize BeginFrame()
    {
        return GetInternals()->head;
    }
    inline void EndFrame(usize frame)
    {
        StackAllocatorHeader *header = GetInternals();
        header->head = frame;
        header->currPage = GetPage(frame / header->pageSize);
    }
    inline IAllocator AsAllocator()
    {
        return IAllocator(payload, &StackAllocator_Allocate, &StackAllocator_Free);
    }
};

inline void* StackAllocator_Allocate(void* instance, usize bytes)
{
    StackAllocator self = StackAllocator(instance);
    StackAllocatorHeader *header = self.GetInternals();

    if (bytes >= header->pageSize)
    {
        return NULL;
    }
    usize posInCurrPage = header->head % header->pageSize;
    usize spaceLeftInCurrPage = header->pageSize - posInCurrPage;
    if (spaceLeftInCurrPage < bytes)
    {
        header->head += spaceLeftInCurrPage;
        if (header->currPage->nextStack == NULL)
        {
            header->currPage->nextStack = (StackPageHeader *)header->baseAllocator.Allocate(sizeof(StackPageHeader) + header->pageSize);
            header->currPage->nextStack->nextStack = NULL;
        }
        header->currPage = header->currPage->nextStack;
    }
    u8 *result = (u8 *)&header->currPage[1];
    result += posInCurrPage;
    header->head += bytes;

    return result;
}
inline void StackAllocator_Free(void* instance, void* ptr)
{

}