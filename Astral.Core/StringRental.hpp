#pragma once
#include "string.hpp"
#include "Array.hpp"
#include "ArenaAllocator.hpp"
#include "DenseSet.hpp"

struct StringRentalBuffer
{
    IAllocator allocator;
    collections::DenseSet<collections::List<string>> stringPool;

    inline StringRentalBuffer()
    {
        stringPool = collections::DenseSet<collections::List<string>>();
    }
    inline StringRentalBuffer(IAllocator allocator)
    {
        this->allocator = allocator;
        stringPool = collections::DenseSet<collections::List<string>>(allocator, collections::List<string>());
    }
    inline string Rent(text str, usize lengthNoNullTerminator)
    {
        usize len = lengthNoNullTerminator;
        //each index of stringPool doubles string length
        //at 0: 16 chars
        //at 1: 32 chars
        //at 2: 64 chars
        //and so on

        usize index = 0;
        usize size = 16;
        while (size <= len + 1)
        {
            size *= 2;
            index++;
        }
        collections::List<string> *stack = stringPool.Get(index);
        string result;
        if (stack == NULL || stack->ptr == NULL || stack->count == 0)
        {
            result = string(allocator, size + 1);
        }
        else
        {
            result = *stack->Get(stack->count - 1);
            stack->RemoveAt_Swap(stack->count - 1);
        }
        result.length = len + 1; //+1 for the null terminator
        memcpy(result.buffer, str, len);
        result.buffer[len] = '\0';

        return result;
    }
    inline string Rent(text str)
    {
        usize len = strlen(str);
        return Rent(str, len);
    }
    inline void Return(string str)
    {
        usize index = 0;
        usize size = 16;
        while (size <= str.length)
        {
            size *= 2;
            index++;
        }
        collections::List<string> *stack = stringPool.Get(index);
        if (stack == NULL || stack->ptr == NULL)
        {
            stack = stringPool.Insert(index, collections::List<string>(allocator));
        }
        stack->Add(str);
    }
    inline void deinit()
    {
        for (u32 i = 0; i < stringPool.capacity; i++)
        {
            if (stringPool[i].ptr != NULL)
            {
                for (u32 j = 0; j < stringPool[j].count; j++)
                {
                    stringPool[i].ptr[j].deinit();
                }
                stringPool[i].deinit();
            }
        }
        stringPool.deinit();
    }
};