#pragma once
#include "string.hpp"
#include "array.hpp"
#include "ArenaAllocator.hpp"
#include "denseset.hpp"

struct StringRentalBuffer
{
    ArenaAllocator arena;
    collections::denseset<collections::vector<string>> stringPool;

    inline StringRentalBuffer()
    {
        arena = ArenaAllocator();
        stringPool = collections::denseset<collections::vector<string>>();
    }
    inline StringRentalBuffer(IAllocator allocator)
    {
        arena = ArenaAllocator(allocator);
        stringPool = collections::denseset<collections::vector<string>>(arena.AsAllocator(), collections::vector<string>());
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
        collections::vector<string> *stack = stringPool.Get(index);
        string result;
        if (stack == NULL || stack->ptr == NULL || stack->count == 0)
        {
            result = string(arena.AsAllocator(), size + 1);
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
        collections::vector<string> *stack = stringPool.Get(index);
        if (stack == NULL || stack->ptr == NULL)
        {
            stack = stringPool.Insert(index, collections::vector<string>(arena.AsAllocator()));
        }
        stack->Add(str);
    }
    inline void deinit()
    {
        arena.deinit();
    }
};