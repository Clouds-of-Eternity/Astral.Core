#pragma once
#include "Array.hpp"
#include "string.hpp"
#include <stdio.h>

struct IDataStream;
enum DataStreamJumpRelative
{
    DataStreamJumpRelative_StreamStart,
    DataStreamJumpRelative_StreamCurrentPos,
    DataStreamJumpRelative_StreamEnd
};

def_delegate(IDataStreamReadFunc, usize, void *self, void *output, usize elementSize, usize readCount);
def_delegate(IDataStreamJumpFunc, bool, void *self, i64 jumpOffset, DataStreamJumpRelative relative);
def_delegate(IDataStreamGetCurrPosFunc, usize, void *self);
def_delegate(IDataStreamReadStringFunc, string, void *self, IAllocator allocator);
def_delegate(IDataStreamPassStringFunc, void, void *self);

def_delegate(IDataStreamWriteFunc, void, void *self, const void *value, usize elementSize, usize writeCount);

struct IDataStream
{
    void *instance;
    IDataStreamReadFunc readFunc;
    IDataStreamJumpFunc jumpFunc;
    IDataStreamGetCurrPosFunc getCurrPosFunc;
    IDataStreamReadStringFunc readStringFunc;
    IDataStreamPassStringFunc passStringFunc;

    IDataStreamWriteFunc writeFunc;

    template<typename T>
    inline void Write(T value)
    {
        writeFunc(instance, &value, sizeof(T), 1);
    }
    template<typename T>
    inline void WriteArray(T *values, usize count)
    {
        writeFunc(instance, values, sizeof(T), count);
    }
    template<typename T>
    inline void WriteArray(collections::Array<T> array)
    {
        writeFunc(instance, array.data, sizeof(T), array.length);
    }
    inline void WriteString(string str)
    {
        writeFunc(instance, str.buffer, 1, str.length);
    }
    inline void WriteCharSlice(CharSlice str)
    {
        const char nullTerm = '\0';
        writeFunc(instance, str.buffer, 1, str.length);
        writeFunc(instance, &nullTerm, 1, 1);
    }
    inline void WriteText(text str)
    {
        writeFunc(instance, str, 1, strlen(str) + 1);
    }
    inline void WriteByte(u8 value)
    {
        writeFunc(instance, &value, 1, 1);
    }

    template<typename T>
    inline T Read()
    {
        T result;
        readFunc(instance, &result, sizeof(T), 1);
        return result;
    }
    template<typename T>
    inline collections::Array<T> ReadArray(IAllocator allocator, usize count)
    {
        collections::Array<T> result = collections::Array<T>(allocator, count);
        readFunc(instance, result.data, sizeof(T), count);
        return result;
    }
    inline string ReadString(IAllocator allocator)
    {
        return readStringFunc(instance, allocator);
    }
    inline void PassString()
    {
        passStringFunc(instance);
    }
    inline u8 ReadByte()
    {
        u8 result;
        readFunc(instance, &result, 1, 1);
        return result;
    }
    inline void ReadByteArray(u8 *out, usize count)
    {
        readFunc(instance, out, 1, count);
    }
    inline void Jump(i64 jumpOffset, DataStreamJumpRelative relative)
    {
        jumpFunc(instance, jumpOffset, relative);
    }
};

inline usize FILE_Read(void *self, void *output, usize elementSize, usize readCount)
{
    FILE *fs = (FILE *)self;
    return fread(output, elementSize, readCount, fs);
}
inline bool FILE_Jump(void *self, i64 jumpOffset, DataStreamJumpRelative relative)
{
    FILE *fs = (FILE *)self;
    return fseek(fs, jumpOffset, (int)relative) == 0;
}
inline string FILE_ReadString(void *self, IAllocator allocator)
{
    FILE *fs = (FILE *)self;
    long currentPos = ftell(fs);

    usize size = 1;
    while (true)
    {
        i32 result = fgetc(fs);
        if (result == 0 || result == -1)
        {
            break;
        }
        size++;
    }
    string str = string(allocator, size);
    fseek(fs, currentPos, SEEK_SET);
    fread(str.buffer, 1, size, fs);
    return str;
}
inline void FILE_PassString(void *self)
{
    FILE *fs = (FILE *)self;

    usize size = 0;
    while (true)
    {
        i32 result = fgetc(fs);
        if (result == 0 || result == -1)
        {
            break;
        }
    }
}
inline usize FILE_GetCurrPos(void *self)
{
    FILE *fs = (FILE *)self;
    return (usize)ftell(fs);
}
inline void FILE_Write(void *self, const void *value, usize elementSize, usize writeCount)
{
    FILE *fs = (FILE *)self;
    fwrite(value, elementSize, writeCount, fs);
}
inline IDataStream GetFileDataStream(FILE *fs)
{
    IDataStream result = {};
    result.instance = fs;
    result.readFunc = &FILE_Read;
    result.readStringFunc = &FILE_ReadString;
    result.jumpFunc = &FILE_Jump;
    result.getCurrPosFunc = &FILE_GetCurrPos;
    return result;
}
inline IDataStream GetWriteFileDataStream(FILE *fs)
{
    IDataStream result = {};
    result.instance = fs;
    result.jumpFunc = &FILE_Jump;
    result.getCurrPosFunc = &FILE_GetCurrPos;
    result.writeFunc = &FILE_Write;
    return result;
}