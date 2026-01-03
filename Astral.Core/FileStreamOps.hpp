#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "string.hpp"
#include "UTF8Utils.hpp"

struct FileStreamReader
{
    FILE *fs;

    inline FileStreamReader()
    {
        fs = NULL;
    }
    inline FileStreamReader(FILE *fs)
    {
        this->fs = fs;
    }
    
    template<typename T>
    inline T Read()
    {
        T result;
        fread(&result, sizeof(T), 1, fs);
        return result;
    }
    inline u8 ReadByte()
    {
        return Read<u8>();
    }
    template<typename T>
    inline collections::Array<T> ReadArray(IAllocator allocator, usize count)
    {
        collections::Array<T> result = collections::Array<T>(allocator, count);
        fread(result.data, sizeof(T), result.length, fs);
        return result;
    }
    inline void ReadByteArray(u8 *out, usize count)
    {
        fread(out, 1, count, fs);
    }
    inline string ReadString(IAllocator allocator)
    {
        collections::vector<char> chars = collections::vector<char>(GetCAllocator());
        while (true)
        {
            int c = fgetc(fs);
            if (c == EOF || c == '\0')
            {
                break;
            }
            chars.Add((char)c);
        }
        string result = string(allocator, chars.ptr, chars.count);
        chars.deinit();
        return result;
    }
    /// @brief Advances past a string without reading it
    inline void PassString()
    {
        while (true)
        {
            int c = fgetc(fs);
            if (c == EOF || c == '\0')
            {
                break;
            }
        }
    }
};
struct FileStreamWriter
{
    FILE *fs;

    inline FileStreamWriter()
    {
        fs = NULL;
    }
    inline FileStreamWriter(FILE *fileStream)
    {
        this->fs = fileStream;
    }
    template<typename T>
    inline void Write(T instance)
    {
        fwrite(&instance, sizeof(T), 1, fs);
    }
    inline void WriteArray(const void *data, usize length)
    {
        fwrite(data, 1, length, fs);
    }
    inline void WriteByte(u8 byte)
    {
        fwrite(&byte, 1, 1, fs);
    }

    inline void WriteEmpty(usize length)
    {
        u8 none = 0;
        fwrite(&none, 1, length, fs);
    }
    inline void WriteStringANSItoU8(string str)
    {
        for (usize i = 0; i < str.length; i++)
        {
            char chars[4];
            u8 len = CharPointToUTF8(str.buffer[i], chars);
            WriteArray(chars, len);
        }
    }
    inline void WriteString(string str)
    {
        WriteArray(str.buffer, str.length);
    }
    inline void WriteText(text str)
    {
        usize i = 0;
        while(str[i] != '\0')
        {
            WriteByte(str[i++]);
        }
        WriteByte(0);
    }
};