#pragma once
#include "./List.h"
#include "./UTF8.h"
#include "./Strings.h"

#ifndef READ_FILE_BUFFER_SIZE
#define READ_FILE_BUFFER_SIZE 1024
#endif

typedef struct ByteStreamReader
{
    const uint8_t *stream;
    size_t position;
    size_t size;

} ByteStreamReader;

inline ByteStreamReader ByteStreamReader_Empty()
{
    const ByteStreamReader result = {};
    return result;
}
inline ByteStreamReader ByteStreamReader_Create(const uint8_t *bytes, size_t startPosition, size_t size)
{
    const ByteStreamReader result = {bytes, startPosition, size};
    return result;
}

inline const void *ByteStreamReader_Read(ByteStreamReader *self, size_t size)
{
    if (self->position + size > self->size)
    {
        return NULL;
    }
    const void *ptr = &self->stream[self->position];
    self->position += size;
    return ptr;
}
inline uint8_t ByteStreamReader_ReadByte(ByteStreamReader *self)
{
    if (self->position + 1 > self->size)
    {
        return 0;
    }
    uint8_t result = self->stream[self->position];
    self->position += 1;
    return result;
}
inline Array ByteStreamReader_ReadArray(ByteStreamReader *self, IAllocator allocator, size_t itemSize, size_t count)
{
    Array result = Array_Create(allocator, itemSize, count);
    memcpy(result.ptr, &self->stream[self->position], itemSize * count);
    self->position += itemSize * count;
    return result;
}
inline void ByteStreamReader_ReadByteArray(ByteStreamReader *self, uint8_t *out, size_t count)
{
    memcpy(out, &self->stream[self->position], count);
    self->position += count;
}
inline uint32_t ByteStreamReader_ReadUTF8(ByteStreamReader *self)
{
    return UTF8GetCharPoint((const char*)self->stream, &self->position);
}
inline CharSlice ByteStreamReader_GetCharSlice(ByteStreamReader *self)
{
    size_t length = 0;
    while (self->stream[self->position + length] != 0)
    {
        length++;
        if (self->position + length >= self->size)
        {
            return CharSliceEmpty();
        }
    }
    CharSlice result;
    result.buffer = (const char *)&self->stream[self->position];
    result.length = length + 1;
    self->position += length + 1;
    return result;
}
inline void ByteStreamReader_PassString(ByteStreamReader *self)
{
    size_t length = 0;
    while (self->stream[self->position + length] != 0)
    {
        length++;
        if (self->position + length >= self->size)
        {
            return;
        }
    }
    self->position += length + 1;
}
inline string ByteStreamReader_GetString(ByteStreamReader *self, IAllocator allocator)
{
    return StringFromCharSlice(allocator, ByteStreamReader_GetCharSlice(self));
}
#define BS_READ(stream, type) *(const type *)ByteStreamReader_Read(&stream, sizeof(type))
#define BSPTR_READ(stream, type) *(const type *)ByteStreamReader_Read(stream, sizeof(type))

typedef struct ByteStreamWriter
{
    List bytes;
} ByteStreamWriter;

inline ByteStreamWriter ByteStreamWriter_Empty()
{
    const ByteStreamWriter result = {};
    return result;
}
inline ByteStreamWriter ByteStreamWriter_Create(IAllocator allocator)
{
    const ByteStreamWriter result = {List_Create(allocator, 1)};
    return result;
}
inline void ByteStreamWriter_Write(ByteStreamWriter *self, const void *item, size_t size)
{
    List_EnsureArrayCapacity(&self->bytes, self->bytes.count + size);
    memcpy((uint8_t *)self->bytes.ptr + self->bytes.count, item, size);
    self->bytes.count += size;
}
inline void ByteStreamWriter_WriteArray(ByteStreamWriter *self, Array array)
{
    ByteStreamWriter_Write(self, array.ptr, array.itemSize * array.length);
}
inline void ByteStreamWriter_WriteByte(ByteStreamWriter *self, uint8_t byte)
{
    List_Add(&self->bytes, &byte);
}
inline void ByteStreamWriter_WriteEmpty(ByteStreamWriter *self, size_t length)
{
    List_EnsureArrayCapacity(&self->bytes, self->bytes.count + length);
    memset(self->bytes.ptr + self->bytes.count, 0, length);
    self->bytes.count += length;
}
inline void ByteStreamWriter_WriteStringANSItoU8(ByteStreamWriter *self, string str)
{
    for (size_t i = 0; i < str.length; i++)
    {
        char chars[4];
        uint8_t len = CharPointToUTF8(str.buffer[i], chars);
        ByteStreamWriter_Write(self, chars, len);
    }
}
inline void ByteStreamWriter_WriteString(ByteStreamWriter *self, string str)
{
    ByteStreamWriter_Write(self, str.buffer, str.length);
}
inline void ByteStreamWriter_WriteCharSlice(ByteStreamWriter *self, CharSlice str)
{
    ByteStreamWriter_Write(self, str.buffer, str.length);
}
inline void ByteStreamWriter_WriteText(ByteStreamWriter *self, const char *text)
{
    ByteStreamWriter_Write(self, text, strlen(text) + 1);
}
inline void ByteStreamWriter_Clear(ByteStreamWriter *self)
{
    List_Clear(&self->bytes);
}
inline void ByteStreamWriter_Deinit(ByteStreamWriter *self)
{
    List_Deinit(&self->bytes);
}
inline ByteStreamReader ByteStreamWriter_ToReader(ByteStreamWriter *self)
{
    return ByteStreamReader_Create((const uint8_t *)self->bytes.ptr, 0, self->bytes.count);
}

inline void ByteStreamWriter_WriteFile(ByteStreamWriter *self, FILE* file)
{
    assert(file);

    uint8_t readBuffer[READ_FILE_BUFFER_SIZE];
    uint32_t bytesRead = 0;

    while ((bytesRead = fread(readBuffer, 1, READ_FILE_BUFFER_SIZE, file)) > 0) 
    {
        ByteStreamWriter_Write(self, readBuffer, bytesRead);
    }
}

inline void *ByteStreamWriter_CloneBytes(IAllocator newAllocator, ByteStreamWriter *self)
{
    void *result = IAllocator_Allocate(newAllocator, self->bytes.count);
    memcpy(result, self->bytes.ptr, self->bytes.count);
    return result;
}