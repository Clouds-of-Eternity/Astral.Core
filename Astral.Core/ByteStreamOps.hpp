#include "io.hpp"
#include "assert.h"

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
    usize position;
    
};