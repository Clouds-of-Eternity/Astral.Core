#pragma once
#include "StackAllocator.hpp"
#include "List.hpp"
#include <string.h>

template <typename T>
struct ObjectPool
{
    def_delegate(InstanceGeneratorFunc, T, IAllocator);

    void **brackets;
    u32 numBrackets;
    u32 bracketCapacity;
    collections::List<u32> freeSlots;
    InstanceGeneratorFunc generatorFunc;

    inline ObjectPool()
    {
        bracketCapacity = 0;
        numBrackets = 0;
        brackets = NULL;
        freeSlots = collections::List<u32>();
    }
    inline ObjectPool(IAllocator allocator, InstanceGeneratorFunc generatorFunc)
    {
        this->generatorFunc = generatorFunc;
        bracketCapacity = 64;
        numBrackets = 1;
        brackets = (void **)allocator.Allocate(sizeof(void *));
        brackets[0] = allocator.Allocate(sizeof(u32) + sizeof(T) * bracketCapacity);
        *((u32 *)brackets[0]) = 0;
        freeSlots = collections::List<u32>(allocator);
    }
    inline ObjectPool(IAllocator allocator,  InstanceGeneratorFunc generatorFunc, u32 bracketsCapacity)
    {
        this->generatorFunc = generatorFunc;
        bracketCapacity = bracketsCapacity;
        brackets = (void **)allocator.Allocate(sizeof(void *));
        brackets[0] = allocator.Allocate(sizeof(u32) + sizeof(T) * bracketCapacity);
        *((u32 *)brackets[0]) = 0;
        freeSlots = collections::List<u32>(allocator);
    }

    inline T *Rent(u32 &borrowAddress)
    {
        if (freeSlots.count > 0)
        {
            borrowAddress = freeSlots.Pop();
            u32 bracketIndex = borrowAddress / bracketCapacity;
            u32 borrowIndex = borrowAddress % bracketCapacity;

            u8 *dataStart = (u8 *)brackets[bracketIndex] + sizeof(u32);
            return &((T*)dataStart)[borrowIndex];
        }
        for (u32 i = 0; i < numBrackets; i++)
        {
            u32 *bracketOccupancyPtr = (u32 *)brackets[i];
            u32 bracketOccupancy = *bracketOccupancyPtr;
            if (bracketOccupancy < bracketCapacity)
            {
                u8 *dataStart = (u8 *)brackets[i] + sizeof(u32);
                borrowAddress = bracketOccupancy + i * bracketCapacity;
                *bracketOccupancyPtr += 1;
                ((T*)dataStart)[bracketOccupancy] = generatorFunc(freeSlots.allocator);
                return &((T*)dataStart)[bracketOccupancy];
            }
        }
        //all brackets occupied, must resize
        void **newPtr = (void **)this->freeSlots.allocator.Allocate(sizeof(void *) * (numBrackets + 1));
        memcpy(newPtr, brackets, sizeof(void *) * numBrackets);
        newPtr[numBrackets] = this->freeSlots.allocator.Allocate(sizeof(u32) + sizeof(T) * bracketCapacity);
        
        brackets = newPtr;
        *((u32 *)brackets[numBrackets]) = 1;
        T *result = (T *)((u8 *)brackets[numBrackets] + sizeof(u32));
        *result = generatorFunc(freeSlots.allocator);

        numBrackets += 1;
        return result;
    }
    inline void Return(u32 borrowAddress)
    {
        freeSlots.Add(borrowAddress);
    }
    inline void deinit()
    {
        for (u32 i = 0; i < numBrackets; i++)
        {
            this->freeSlots.allocator.Free(brackets[i]);
        }
        this->freeSlots.allocator.freeFunction(this->freeSlots.allocator.instance, brackets);
        freeSlots.deinit();
    }
};