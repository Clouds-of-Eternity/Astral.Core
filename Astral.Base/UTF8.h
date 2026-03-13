#pragma once
#include "Array.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

inline bool IsValidUTF8(const char *utf8, size_t lengthToCheck)
{
    size_t index = 0;
    while (index < lengthToCheck)
    {
        char startingByte = utf8[index];
        if ((startingByte >> 7) == 0)
        {
            index += 1;
        }
        else if ((startingByte >> 5) == 0b110)
        {
            index += 2;
        }
        else if ((startingByte >> 4) == 0b1110)
        {
            index += 3;
        }
        else if ((startingByte >> 3) == 0b11110)
        {
            index += 4;
        }
        else
        {
            return false;
        }
    }
    return true;
}

inline bool UTF8GetCharPointAt(const char *utf8, size_t index, uint32_t *result)
{
    char startingByte = utf8[index];
    if ((startingByte & 0b11111000) == 0b11110000)
    {
        *result = ((startingByte & 0b00000111) << (6 + 6 + 6)) | ((utf8[index + 1] & 0b00111111) << (6 + 6)) | ((utf8[index + 2] & 0b00111111) << 6) | ((utf8[index + 3] & 0b00111111));
        return true;
    }
    else if ((startingByte & 0b11110000) == 0b11100000)
    {
        *result = ((startingByte & 0b00001111) << (6 + 6)) | ((utf8[index + 1] & 0b00111111) << 6) | ((utf8[index + 2] & 0b00111111));
        return true;
    }
    else if ((startingByte & 0b11100000) == 0b11000000)
    {
        *result = ((startingByte & 0b00011111) << 6) | (utf8[index + 1] & 0b00111111);
        return true;
    }
    else if ((startingByte >> 7) == 0)
    {
        *result = startingByte;
        return true;
    }

    return false;
}
inline uint32_t UTF8GetCharPoint(const char *utf8, size_t *index)
{
    uint32_t result = 0;
    char startingByte = utf8[*index];
    if ((startingByte & 0b11111000) == 0b11110000)
    {
        result = ((startingByte & 0b00000111) << (6 + 6 + 6)) | ((utf8[*index + 1] & 0b00111111) << (6 + 6)) | ((utf8[*index + 2] & 0b00111111) << 6) | ((utf8[*index + 3] & 0b00111111));
        *index += 4;
    }
    else if ((startingByte & 0b11110000) == 0b11100000)
    {
        result = ((startingByte & 0b00001111) << (6 + 6)) | ((utf8[*index + 1] & 0b00111111) << 6) | ((utf8[*index + 2] & 0b00111111));
        *index += 3;
    }
    else if ((startingByte & 0b11100000) == 0b11000000)
    {
        result = ((startingByte & 0b00011111) << 6) | (utf8[*index + 1] & 0b00111111);
        *index += 2;
    }
    else if ((startingByte >> 7) == 0)
    {
        result = startingByte;
        *index += 1;
    }
    return result;
}
inline void ByteToBits(uint8_t byte, char* results)
{
    results[0] = (byte >> 7) ? '1' : '0';
    results[1] = ((byte >> 6) & 1) ? '1' : '0';
    results[2] = ((byte >> 5) & 1) ? '1' : '0';
    results[3] = ((byte >> 4) & 1) ? '1' : '0';
    results[4] = ((byte >> 3) & 1) ? '1' : '0';
    results[5] = ((byte >> 2) & 1) ? '1' : '0';
    results[6] = ((byte >> 1) & 1) ? '1' : '0';
    results[7] = (byte & 1) ? '1' : '0';
}

inline uint8_t CharPointToUTF8(uint32_t charPoint, char *output)
{
    if (charPoint <= 0x7F) {
        output[0] = charPoint;
        return 1;
    }
    if (charPoint <= 0x7FF) {
        output[0] = 0xC0 | (charPoint >> 6);            /* 110xxxxx */
        output[1] = 0x80 | (charPoint & 0x3F);          /* 10xxxxxx */
        return 2;
    }
    if (charPoint <= 0xFFFF) {
        output[0] = 0xE0 | (charPoint >> 12);           /* 1110xxxx */
        output[1] = 0x80 | ((charPoint >> 6) & 0x3F);   /* 10xxxxxx */
        output[2] = 0x80 | (charPoint & 0x3F);          /* 10xxxxxx */
        return 3;
    }
    if (charPoint <= 0x10FFFF) {
        output[0] = 0xF0 | (charPoint >> 18);           /* 11110xxx */
        output[1] = 0x80 | ((charPoint >> 12) & 0x3F);  /* 10xxxxxx */
        output[2] = 0x80 | ((charPoint >> 6) & 0x3F);   /* 10xxxxxx */
        output[3] = 0x80 | (charPoint & 0x3F);          /* 10xxxxxx */
        return 4;
    }
    return 0;
}

inline uint32_t *UTF8To32(IAllocator alloc, const char *inputText, size_t *outputStringLength)
{
    uint32_t *maxSizeString = (uint32_t *)IAllocator_Allocate(alloc, 4 * (strlen(inputText) + 1));
    size_t index = 0;
    size_t i = 0;
    while (true)
    {
        uint32_t result = UTF8GetCharPoint(inputText, &index);
        maxSizeString[i] = result;
        i++;
        if (result == 0)
        {
            break;
        }
    }
    maxSizeString[i] = 0;
    *outputStringLength = i + 1;
    return maxSizeString;
}
inline uint8_t *UTF32To8(IAllocator alloc, const uint32_t *inputText, size_t inputStringLength, size_t *outputStringLength)
{
    uint8_t *maxSizeString = (uint8_t *)IAllocator_Allocate(alloc, inputStringLength * 4 + 1);
    size_t index = 0;
    for (size_t i = 0; i < inputStringLength; i++)
    {
        char output[4];
        uint8_t advance = CharPointToUTF8(inputText[i], output);
        if (advance == 1)
        {
            maxSizeString[index] = output[0];
        }
        else if (advance == 2)
        {
            maxSizeString[index] = output[1];
        }
        else if (advance == 3)
        {
            maxSizeString[index] = output[2];
        }
        else if (advance == 4)
        {
            maxSizeString[index] = output[3];
        }
        index += advance;
    }
    maxSizeString[index] = '\0';
    *outputStringLength = index + 1;
    return maxSizeString;
}
inline Array UTF8To32Array(IAllocator alloc, const char *inputText)
{
    size_t arrayLength;
    uint32_t *ptr = UTF8To32(alloc, inputText, &arrayLength);
    return Array_CreateFromExisting(alloc, ptr, sizeof(uint32_t), arrayLength);
}
inline uint8_t *UTF32ArrayTo8(IAllocator alloc, Array inputText)
{
    size_t outputStrLen;
    return UTF32To8(alloc, (const uint32_t *)inputText.ptr, inputText.length, &outputStrLen);
}
inline Array UTF32ArrayTo8Array(IAllocator alloc, Array inputText)
{
    Array result;
    result.allocator = alloc;
    result.ptr = UTF32To8(alloc, (const uint32_t *)inputText.ptr, inputText.length, &result.length);

    return result;
}
inline wchar_t *UTF8ToWChar(IAllocator alloc, const char *inputText)
{
    wchar_t *maxSizeString = (wchar_t *)IAllocator_Allocate(alloc, 4 * (strlen(inputText) + 1));
    size_t index = 0;
    size_t i = 0;
    while (true)
    {
        uint32_t result = UTF8GetCharPoint(inputText, &index);
        maxSizeString[i] = (wchar_t)result;
        i++;
        if (result == 0)
        {
            break;
        }
    }
    maxSizeString[i] = 0;
    return maxSizeString;
}
inline uint8_t *WCharToUTF8(IAllocator alloc, const wchar_t *inputText)
{
    size_t len = wcslen(inputText);
    uint8_t *maxSizeString = (uint8_t *)IAllocator_Allocate(alloc, len * 4 + 1);
    size_t index = 0;
    for (size_t i = 0; i < len; i++)
    {
        char output[4];
        uint8_t advance = CharPointToUTF8((uint32_t)inputText[i], output);
        if (advance == 1)
        {
            maxSizeString[index] = output[0];
        }
        else if (advance == 2)
        {
            maxSizeString[index] = output[1];
        }
        else if (advance == 3)
        {
            maxSizeString[index] = output[2];
        }
        else if (advance == 4)
        {
            maxSizeString[index] = output[3];
        }
        index += advance;
    }
    maxSizeString[index] = '\0';
    return maxSizeString;
}