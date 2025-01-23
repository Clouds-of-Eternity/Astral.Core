#pragma once
#include "vector.hpp"
#include "io.hpp"
#include "UTF8Utils.hpp"

typedef collections::vector<u8> ByteStream;

struct ByteStreamReader
{
    usize position;
    usize size;
    u8* stream;

    inline ByteStreamReader()
    {
        position = 0;
        stream = NULL;
    }
    inline ByteStreamReader(u8* byteStream, usize size, usize pos)
    {
        this->position = pos;
        this->size = size;
        this->stream = byteStream;
    }
    
    template<typename T>
    inline T Read()
    {
        if (position + sizeof(T) > size)
        {
            return T();
        }
        T *ptr = (T *)&stream[position];
        position += sizeof(T);
        return *ptr;
    }
    inline u8 ReadByte()
    {
        if (position + 1 > size)
        {
            return 0;
        }
        return stream[position++];
    }
    template<typename T>
    inline collections::Array<T> ReadArray(IAllocator allocator, usize count)
    {
        collections::Array<T> result = collections::Array<T>(allocator, count);
        memcpy(result.data, &stream[position], sizeof(T) * count);
        position += sizeof(T) * count;
        return result;
    }
    inline void ReadByteArray(u8 *out, usize count)
    {
        memcpy(out, &stream[position], count);
        position += count;
    }
    inline u32 ReadUTF8()
    {
        return UTF8GetCharPoint((text)stream, &position);
    }
    inline string ReadString(IAllocator allocator)
    {
        usize length = 0;
        while (stream[position + length] != 0)
        {
            length++;
            if (position + length >= size)
            {
                return string();
            }
        }
        string result = string(allocator, length + 1);
        memcpy(result.buffer, &stream[position], length);
        result.buffer[length] = '\0';
        position += length + 1;
        return result;
    }
};
struct ByteStreamWriter
{
    ByteStream bytes;

    inline ByteStreamWriter()
    {
        bytes = ByteStream();
    }
    inline ByteStreamWriter(IAllocator allocator)
    {
        bytes = ByteStream(allocator);
    }
    template<typename T>
    inline void Write(T instance)
    {
        bytes.EnsureArrayCapacity(bytes.count + sizeof(T));
        *((T *)&bytes.ptr[bytes.count]) = instance;
        bytes.count += sizeof(T);
    }
    inline void WriteArray(const void *data, usize length)
    {
        bytes.EnsureArrayCapacity(bytes.count + length);
        memcpy(bytes.ptr + bytes.count, data, length);
        bytes.count += length;
    }
    inline void WriteByte(u8 byte)
    {
        bytes.Add(byte);
    }
    inline void WriteEmpty(usize length)
    {
        bytes.EnsureArrayCapacity(bytes.count + length);
        memset(bytes.ptr + bytes.count, 0, length);
        bytes.count += length;
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
    inline void Clear()
    {
        bytes.Clear();
    }
    inline void deinit()
    {
        bytes.deinit();
    }
};