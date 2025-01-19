#pragma once
#include "vector.hpp"
#include "io.hpp"
#include "assert.h"

typedef collections::vector<u8> ByteStream;

struct ByteStreamReader
{
    usize position;
    u8* stream;

    inline ByteStreamReader()
    {
        position = 0;
        stream = NULL;
    }
    inline ByteStreamReader(usize pos, u8* byteStream)
    {
        this->position = pos;
        this->stream = byteStream;
    }
    
    template<typename T>
    inline T Read()
    {
        assert(stream != NULL);
        T *ptr = (T *)&stream[position];
        position += sizeof(T);
        return *ptr;
    }
    inline string ReadString(IAllocator allocator)
    {
        assert(stream != NULL);
        usize length = 0;
        while (stream[position + length] != 0)
        {
            length++;
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
    inline void Clear()
    {
        bytes.Clear();
    }
    inline void deinit()
    {
        bytes.deinit();
    }
};