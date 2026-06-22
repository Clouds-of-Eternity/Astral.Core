#pragma once
#include "./Strings.h"
#include "./List.h"

typedef struct StringPoolBuffer
{
    List strings;
} StringPoolBuffer;

typedef struct StringPool
{
    IAllocator allocator;
    List buffers;
} StringPool;

static inline StringPool StringPool_Create(IAllocator allocator)
{
    StringPool result;
    result.allocator = allocator;
    result.buffers = List_Create(allocator, sizeof(StringPoolBuffer));
    return result;
}
static inline void StringPool_Deinit(StringPool *self)
{
    for (uint32_t i = 0; i < self->buffers.count; i++)
    {
        StringPoolBuffer *buffer = LIST_GETREF(&self->buffers, StringPoolBuffer, i);
        for (uint32_t j = 0; j < buffer->strings.count; j++)
        {
            string *str = LIST_GETREF(&buffer->strings, string, j);
            String_Deinit(str);
        }
        List_Deinit(&buffer->strings);
    }
    List_Deinit(&self->buffers);
}

static inline string StringPool_RentLength(StringPool *self, const char *str, size_t lengthNoNullTerminator)
{
    size_t len = lengthNoNullTerminator;
    
    size_t index = 0;
    size_t size = 16;
    while (size <= len + 1)
    {
        size *= 2;
        index++;
    }
    StringPoolBuffer *stack = LIST_GETREF(&self->buffers, StringPoolBuffer, index);
    string result;
    if (stack == NULL || stack->strings.ptr == NULL)
    {
        result = StringFromLength(self->allocator, size + 1);//string(arena.AsAllocator(), size + 1);
    }
    else
    {
        result = *(string *)List_Pop(&stack->strings);
    }
    result.length = len + 1; //+1 for the null terminator
    memcpy(result.buffer, str, len);
    result.buffer[len] = '\0';

    return result;
}
static inline string StringPool_Rent(StringPool *self, const char *str)
{
    size_t len = strlen(str);
    return StringPool_RentLength(self, str, len);
}
static inline void StringPool_Return(StringPool *self, string str)
{
    size_t index = 0;
    size_t size = 16;
    while (size <= str.length)
    {
        size *= 2;
        index++;
    }
    StringPoolBuffer *stack = LIST_GETREF(&self->buffers, StringPoolBuffer, index);
    if (stack == NULL || stack->strings.ptr == NULL)
    {
        StringPoolBuffer newStack;
        newStack.strings = List_Create(self->allocator, sizeof(List));
        stack = (StringPoolBuffer *)List_InsertOverride(&self->buffers, &newStack, index);
        //stack = stringPool.Insert(index, collections::vector<string>(arena.AsAllocator()));
    }
    List_Add(&stack->strings, &str);
}

StringPool *GetGlobalStringPool();

#define RENTSTR(literal) StringPool_Rent(GetGlobalStringPool(), literal)
#define RETSTR(str) StringPool_Return(GetGlobalStringPool(), str)