#pragma once
#include "string.hpp"
#include "list.hpp"
#include "UTF8Utils.hpp"
#include "stdarg.h"

#ifdef STB_SPRINTF_H_INCLUDE
#ifndef STRINGBUILDER_USE_STB
#define STRINGBUILDER_USE_STB
#define vsnprintf stbsp_vsnprintf
#define vsprintf stbsp_vsprintf
#define snprintf stbsp_snprintf
#define sprintf stbsp_sprintf
#endif
#endif

struct StringBuilder
{
    IAllocator allocator;
    collections::list<char> buffer;

    inline StringBuilder()
    {
        allocator = IAllocator{};
        buffer = collections::list<char>();
    }
    inline StringBuilder(IAllocator allocator)
    {
        this->allocator = allocator;
        buffer = collections::list<char>(allocator);
    }
    inline StringBuilder &Append(text text)
    {
        usize i = 0;
        while (text[i] != '\0')
        {
            buffer.Add(text[i]);
            i++;
        }
        return *this;
    }
    inline StringBuilder &AppendReplace(text text, const char *charsToReplace, char replaceWith)
    {
        usize charsToReplaceLength = strlen(charsToReplace);
        usize i = 0;
        while (text[i] != '\0')
        {
            u32 c = 0;
            while (c < charsToReplaceLength)
            {
                if (text[i] == charsToReplace[c])
                {
                    buffer.Add(replaceWith);
                    i++;
                    c = 0;
                }
                else
                {
                    c++;
                }
            }
            buffer.Add(text[i]);
            i++;
        }
        return *this;
    }
    inline StringBuilder &AppendString(const string str)
    {
        this->Append(str.buffer);
        return *this;
    }
    inline StringBuilder &AppendDeinit(string str)
    {
        this->Append(str.buffer);
        str.deinit();
        return *this;
    }
    inline StringBuilder &AppendLine(text text)
    {
        usize i = 0;
        while (text[i] != '\0')
        {
            buffer.Add(text[i]);
            i++;
        }
        return *this;
    }
    inline StringBuilder &Appendf(text format, ...)
    {
        va_list args;
        va_start(args, format);

        char chars[256];
        i32 len = vsnprintf(chars, 256, format, args);
        buffer.EnsureArrayCapacity(buffer.count + len);
        memcpy(buffer.ptr + buffer.count, chars, len);
        buffer.count += len;

        va_end(args);
        return *this;
    }
    inline StringBuilder &AppendfLong(text format, ...)
    {
        va_list args;
        va_list args2;
        va_start(args, format);
        va_copy(args2, args);
        i32 len = vsnprintf(NULL, 0, format, args);
        va_end(args);
        buffer.EnsureArrayCapacity(buffer.count + len + 1);
        vsnprintf(buffer.ptr + buffer.count, len + 1, format, args2);
        buffer.count += len;
        va_end(args2);
        return *this;
    }
    inline StringBuilder& AppendStringLine(string text)
    {
        this->AppendLine(text.buffer);
        return *this;
    }
    inline StringBuilder &AppendChar(char character)
    {
        this->buffer.Add(character);
        return *this;
    }
    inline StringBuilder &AppendChar32(u32 char32)
    {
        char outputs[4];
        u8 advance = CharPointToUTF8(char32, outputs);
        for (u8 i = 0; i < advance; i++)
        {
            AppendChar(outputs[i]);
        }
        return *this;
    }
    inline usize InsertChar32At(u32 char32, usize at)
    {
        char bytes[4];
        if (char32 <= 0x7F)
        {
            bytes[0] = (char)char32;
            this->buffer.InsertAll(bytes, 1, at);
            return 1;
        }
        else if (char32 <= 0x7FF)
        {
            bytes[0] = (char)(((char32 >> 6) & 0x1F) | 0xC0);
            bytes[1] = (char)(((char32 >> 0) & 0x3F) | 0x80);
            this->buffer.InsertAll(bytes, 2, at);
            return 2;
        }
        else if (char32 <= 0xFFFF)
        {
            bytes[0] = (char)(((char32 >> 12) & 0x0F) | 0xE0);
            bytes[1] = (char)(((char32 >> 6) & 0x3F) | 0x80);
            bytes[2] = (char)(((char32 >> 0) & 0x3F) | 0x80);
            this->buffer.InsertAll(bytes, 3, at);
            return 3;
        }
        else if (char32 <= 0x10FFFF)
        {
            bytes[0] = (char)(((char32 >> 18) & 0x07) | 0xF0);
            bytes[1] = (char)(((char32 >> 12) & 0x3F) | 0x80);
            bytes[2] = (char)(((char32 >> 6) & 0x3F) | 0x80);
            bytes[3] = (char)(((char32 >> 0) & 0x3F) | 0x80);
            this->buffer.InsertAll(bytes, 4, at);
            return 4;
        }
        else
        {
            return 0;
        }
    }
    inline void RemoveCharUTF8At(usize index)
    {
        usize i = index;
        u32 point = UTF8GetCharPoint(this->buffer.ptr, &i);
        if (point != 0)
        {
            usize diff = i - index;
            //remove diff
            this->buffer.RemoveManyAt(index, diff);
        }
    }
    inline string ToString(IAllocator stringAllocator, bool alsoClear = false)
    {
        string str = string(stringAllocator);
        str.length = buffer.count + 1;
        str.buffer = (char*)stringAllocator.Allocate(str.length);
        memcpy(str.buffer, buffer.ptr, buffer.count);
        str.buffer[buffer.count] = '\0';

        if (alsoClear)
        {
            buffer.Clear();
        }
        
        return str;
    }
    inline void Clear()
    {
        buffer.Clear();
    }
    inline void deinit()
    {
        buffer.deinit();
    }
};