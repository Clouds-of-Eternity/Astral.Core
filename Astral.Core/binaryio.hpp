#pragma once
#include "Linxc.h"
#include "stdio.h"
#include "string.hpp"
#include "array.hpp"

template <typename T>
inline void Binary_WriteData(FILE *fs, const T input)
{
    fwrite(&input, sizeof(T), 1, fs);
}

template <typename T>
inline void Binary_WriteArray(FILE *fs, const T *input, usize arrayLength)
{
    Binary_WriteData<usize>(fs, arrayLength);
    fwrite(input, sizeof(T), arrayLength, fs);
}

inline void Binary_WriteString(FILE *fs, const string str)
{
    Binary_WriteArray(fs, str.buffer, str.length);
}

inline void Binary_WriteText(FILE *fs, text str)
{
    u32 index = 0;
    char character;
    while (str[index] != 0)
    {
        character = str[index];
        fwrite(&character, sizeof(char), 1, fs);
        index++;
    }
    character = '\0';
    fwrite(&character, sizeof(char), 1, fs);
}

template <typename T>
inline T Binary_ReadData(FILE *fs)
{
    T result;
    fread(&result, sizeof(T), 1, fs);
    return result;
}

template <typename T>
inline collections::Array<T> Binary_ReadArray(IAllocator alloc, FILE *fs)
{
    usize arrayLength = Binary_ReadData<usize>(fs);
    collections::Array<T> results = collections::Array<T>(alloc, arrayLength);

    fread(results.data, sizeof(T), results.length, fs);

    return results;
}

inline string BinaryReadString(IAllocator allocator, FILE *fs)
{
    long currentPos = ftell(fs);

    usize size = 0;
    while (true)
    {
        i32 result = fgetc(fs);
        if (result != 0 && result != -1)
        {
            size++;
        }
        else
            break;
    }
    string str = string(allocator, size);
    fseek(fs, 0, currentPos);
    fread(str.buffer, 1, size, fs);
    return str;
}