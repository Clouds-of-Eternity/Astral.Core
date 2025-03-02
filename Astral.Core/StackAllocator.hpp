#pragma once
#include "allocators.hpp"
#include <assert.h>

#define KiB_SIZE 1024
#define MiB_SIZE (KiB_SIZE * 1024)

inline void *StackAllocator_Allocate(void *instance, usize bytes);
inline void StackAllocator_Free(void *instance, void *ptr);

enum StackOverflowPolicy
{
    StackOverflowPolicy_ReturnNull,
    StackOverflowPolicy_AssertFalse,
    StackOverflowPolicy_NewPage
};
struct StackData;
struct StackData
{
    void *ptr;
    usize head;
    StackData *nextStack;
};
struct StackAllocatorImpl
{
    //1 usize at the start of the stack pointer is reserved for the stack head.
    //next usize at the start is used as the pointer to the next stack 
    //in case policy is set to NewPage and the stack runs out of space
    StackData firstStack;
    usize stackSize;
    StackOverflowPolicy policy;
    IAllocator baseAllocator;

    inline StackAllocatorImpl()
    {
        firstStack = StackData();
        stackSize = 0;
        policy = StackOverflowPolicy_ReturnNull;
        baseAllocator = {};
    }
    inline StackAllocatorImpl(IAllocator baseAllocator, usize stackSize, StackOverflowPolicy overflowPolicy)
    {
        this->baseAllocator = baseAllocator;
        this->stackSize = stackSize;
        this->policy = overflowPolicy;
        firstStack.ptr = baseAllocator.Allocate(stackSize);
        firstStack.head = 0;
        firstStack.nextStack = NULL;
    }
    inline u32 BeginFrame()
    {
        u32 frame = 0;
        StackData *ptr = &firstStack;
        while (ptr != NULL)
        {
            frame += ptr->head;
            ptr = ptr->nextStack;
        }
        return frame;
    }
    inline void EndFrame(u32 prevFrame)
    {
        StackData *ptr = &firstStack;
        u32 stackIndex = prevFrame / stackSize;
        while (ptr != NULL)
        {
            if (prevFrame < stackSize)
            {
                ptr->head = prevFrame;
                prevFrame = 0;
            }
            else
            {
                prevFrame -= stackSize;
            }
            ptr = ptr->nextStack;
        }
    }
    inline void deinit()
    {
        StackData stack = firstStack;
        while (true)
        {
            this->baseAllocator.Free(stack.ptr);
            if (stack.nextStack != NULL)
            {
                stack = *stack.nextStack;
                stack.nextStack = NULL;
                baseAllocator.Free(stack.nextStack);
            }
            else
            {
                break;
            }
        }
        firstStack = StackData();
        stackSize = 0;
    }
};
struct StackAllocator
{
    StackAllocatorImpl *ptr;

    inline StackAllocator()
    {
        ptr = NULL;
    }
    inline StackAllocator(StackAllocatorImpl *ptr)
    {
        this->ptr = ptr;
    }
    inline StackAllocator(IAllocator baseAllocator, usize stackSize, StackOverflowPolicy overflowPolicy)
    {
        ptr = (StackAllocatorImpl *)baseAllocator.Allocate(sizeof(StackAllocatorImpl));
        *ptr = StackAllocatorImpl(baseAllocator, stackSize, overflowPolicy);
    }
    inline void deinit()
    {
        IAllocator baseAllocator = ptr->baseAllocator;
        ptr->deinit();
        baseAllocator.Free(ptr);
    }
    inline u32 BeginFrame()
    {
        return ptr->BeginFrame();
    }
    inline void EndFrame(u32 frame)
    {
        ptr->EndFrame(frame);
    }
    inline IAllocator AsAllocator()
    {
        return IAllocator(ptr, &StackAllocator_Allocate, &StackAllocator_Free);
    }
};

void* StackAllocator_Allocate(void* instance, usize bytes)
{
    StackAllocatorImpl *impl = (StackAllocatorImpl *)instance;
    if (bytes >= impl->stackSize)
    {
        return NULL;
    }
    StackData *stackToUse = &impl->firstStack;
    while (stackToUse->head + bytes >= impl->stackSize)
    {
        if (impl->policy == StackOverflowPolicy_AssertFalse)
        {
            assert(false);
            return NULL;
        }
        else if (impl->policy == StackOverflowPolicy_ReturnNull)
        {
            return NULL;
        }
        else
        {
            if (stackToUse->nextStack == NULL)
            {
                stackToUse->head = impl->stackSize;
                stackToUse->nextStack = (StackData *)impl->baseAllocator.Allocate(sizeof(StackData));
                stackToUse->nextStack->head = 0;
                stackToUse->nextStack->nextStack = NULL;
                stackToUse->nextStack->ptr = impl->baseAllocator.Allocate(impl->stackSize);
            }
            stackToUse = stackToUse->nextStack;
        }
    }
    void *result = (u8 *)stackToUse->ptr + stackToUse->head;
    stackToUse->head += bytes;
    return result;
}
void StackAllocator_Free(void* instance, void* ptr)
{

}