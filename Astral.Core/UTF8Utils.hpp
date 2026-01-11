#pragma once
#include "Linxc.h"
#include "allocators.hpp"
#include "option.hpp"
#include <stdio.h>
#include <string.h>
#include <wchar.h>

/// Check if a u32 is a valid UTF8 character. If so, returns the byte in which they are valid starting from.
inline bool IsValidUTF8(text utf8, usize lengthToCheck)
{
    usize index = 0;
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
inline option<u32> UTF8GetCharPointAt(text utf8, usize index)
{
    u32 result;
    char startingByte = utf8[index];
    if ((startingByte & 0b11111000) == 0b11110000)
    {
        result = ((startingByte & 0b00000111) << (6 + 6 + 6)) | ((utf8[index + 1] & 0b00111111) << (6 + 6)) | ((utf8[index + 2] & 0b00111111) << 6) | ((utf8[index + 3] & 0b00111111));
    }
    else if ((startingByte & 0b11110000) == 0b11100000)
    {
        result = ((startingByte & 0b00001111) << (6 + 6)) | ((utf8[index + 1] & 0b00111111) << 6) | ((utf8[index + 2] & 0b00111111));
    }
    else if ((startingByte & 0b11100000) == 0b11000000)
    {
        result = ((startingByte & 0b00011111) << 6) | (utf8[index + 1] & 0b00111111);
    }
    else if ((startingByte >> 7) == 0)
    {
        result = startingByte;
    }
    else
        return option<u32>();
    return option<u32>(result);
    /*u32 result = 0;
    //is single byte
    char startingByte = utf8[index];
    if ((startingByte >> 7) == 0)
    {
        result = startingByte & 0b01111111;
    }
    else if ((startingByte & 0b11100000) == 0b11000000)
    {
        //-15703 = (u16)49833
        result = (startingByte & 0b00011111) | 0b11000000;
        result = (result << 8) | ((utf8[index + 1] & 0b00111111) | 0b10000000);
    }
    else if ((startingByte & 0b11110000) == 0b11100000)
    {
        result = (startingByte & 0b00001111) | 0b11100000;
        result = (result << 16) | (((utf8[index + 1] & 0b00111111) | 0b10000000) << 8) | ((utf8[index + 2] & 0b00111111) | 0b10000000);
    }
    else if ((startingByte & 0b11111000) == 0b11110000)
    {
        result = (startingByte & 0b00000111) | 0b11110000;
        result = (result << 24) | (((utf8[index + 1] & 0b00111111) | 0b10000000) << 16) | (((utf8[index + 2] & 0b00111111) | 0b10000000) << 8) | ((utf8[index + 3] & 0b00111111) | 0b10000000);
    }
    else
    {
        return option<u32>();
    }
    return option<u32>(result);*/
}
inline u32 UTF8GetCharPoint(text utf8, usize *index)
{
    u32 result;
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
    return 0;
    /*u32 result = 0;
    //is single byte
    char startingByte = utf8[*index];
    if ((startingByte >> 7) == 0)
    {
        result = startingByte & 0b01111111;
        *index += 1;
    }
    else if ((startingByte & 0b11100000) == 0b11000000)
    {
        result = (startingByte & 0b00011111) | 0b11000000;
        result = (result << 8) | ((utf8[*index + 1] & 0b00111111) | 0b10000000);
        *index += 2;
    }
    else if ((startingByte & 0b11110000) == 0b11100000)
    {
        result = (startingByte & 0b00001111) | 0b11100000;
        result = (result << 16) | (((utf8[*index + 1] & 0b00111111) | 0b10000000) << 8) | ((utf8[*index + 2] & 0b00111111) | 0b10000000);
        *index += 3;
    }
    else if ((startingByte & 0b11111000) == 0b11110000)
    {
        result = (startingByte & 0b00000111) | 0b11110000;
        result = (result << 24) | (((utf8[*index + 1] & 0b00111111) | 0b10000000) << 16) | (((utf8[*index + 2] & 0b00111111) | 0b10000000) << 8) | ((utf8[*index + 3] & 0b00111111) | 0b10000000);
        *index += 4;
    }
    return result;*/
}
inline void ByteToBits(u8 byte, char* results)
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
inline u8 CharPointToUTF8(u32 charPoint, char *output)
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
inline wchar_t *UTF8ToWChar(IAllocator alloc, const char *inputText)
{
    wchar_t *maxSizeString = (wchar_t *)alloc.Allocate(4 * strlen(inputText) + 1);
    usize index = 0;
    usize i = 0;
    while (true)
    {
        u32 result = UTF8GetCharPoint(inputText, &index);
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
inline u8 *WCharToUTF8(IAllocator alloc, const wchar_t *inputText)
{
    usize len = wcslen(inputText);
    u8 *maxSizeString = (u8 *)alloc.Allocate(len * 4 + 1);
    usize index = 0;
    for (usize i = 0; i < len; i++)
    {
        char output[4];
        u8 advance = CharPointToUTF8((u32)inputText[i], output);
        if (advance == 1)
        {
            maxSizeString[index] = output[0];
        }
        if (advance == 2)
        {
            maxSizeString[index] = output[1];
        }
        if (advance == 3)
        {
            maxSizeString[index] = output[2];
        }
        if (advance == 4)
        {
            maxSizeString[index] = output[3];
        }
        index += advance;
    }
    maxSizeString[index] = '\0';
    return maxSizeString;
}