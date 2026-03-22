#pragma once
#include "Array.h"
#include "Strings.h"
#include <stdio.h>

typedef struct IDataStream IDataStream;
typedef enum DataStreamJumpRelative
{
    DataStreamJumpRelative_StreamStart,
    DataStreamJumpRelative_StreamCurrentPos,
    DataStreamJumpRelative_StreamEnd
} DataStreamJumpRelative;

def_delegate(IDataStreamReadFunc, size_t, void *self, void *output, size_t elementSize, size_t readCount);
def_delegate(IDataStreamJumpFunc, bool, void *self, int64_t jumpOffset, DataStreamJumpRelative relative);
def_delegate(IDataStreamGetCurrPosFunc, size_t, void *self);
def_delegate(IDataStreamReadStringFunc, string, void *self, IAllocator allocator);
def_delegate(IDataStreamPassStringFunc, void, void *self);

typedef struct IDataStream
{
    void *instance;
    IDataStreamReadFunc readFunc;
    IDataStreamJumpFunc jumpFunc;
    IDataStreamGetCurrPosFunc getCurrPosFunc;
    IDataStreamReadStringFunc readStringFunc;
    IDataStreamPassStringFunc passStringFunc;
} IDataStream;

#define DS_READINTO(varName, streamPtr, type) type varName; streamPtr->readFunc(streamPtr->instance, &varName, sizeof(type), 1)
#define DS_READINTOARRAY(varName, streamPtr, type, allocator, count) Array varName = Array_Create(allocator, sizeof(type), count); streamPtr->readFunc(streamPtr->instance, varName.ptr, sizeof(type), count)

uint32_t IDataStream_ReadU32(IDataStream *self)
{
    DS_READINTO(result, self, uint32_t);
    return result;
}
int32_t IDataStream_ReadI32(IDataStream *self)
{
    DS_READINTO(result, self, int32_t);
    return result;
}

string IDataStream_ReadString(IDataStream *self, IAllocator allocator)
{
    return self->readStringFunc(self->instance, allocator);
}
void IDataStream_PassString(IDataStream *self)
{
    self->passStringFunc(self->instance);
}
uint8_t IDataStream_ReadByte(IDataStream *self)
{
    uint8_t result;
    self->readFunc(self->instance, &result, sizeof(uint8_t), 1);
    return result;
}
void IDataStream_ReadByteArray(IDataStream *self, void *out, size_t count)
{
    self->readFunc(self->instance, out, 1, count);
}
void IDataStream_Jump(IDataStream *self, int64_t jumpOffset, DataStreamJumpRelative relative)
{
    self->jumpFunc(self->instance, jumpOffset, relative);
}

inline size_t FILE_Read(void *self, void *output, size_t elementSize, size_t readCount)
{
    FILE *fs = (FILE *)self;
    return fread(output, elementSize, readCount, fs);
}
inline bool FILE_Jump(void *self, int64_t jumpOffset, DataStreamJumpRelative relative)
{
    FILE *fs = (FILE *)self;
    return fseek(fs, jumpOffset, (int)relative) == 0;
}
inline string FILE_ReadString(void *self, IAllocator allocator)
{
    FILE *fs = (FILE *)self;
    long currentPos = ftell(fs);

    size_t size = 1;
    while (true)
    {
        int32_t result = fgetc(fs);
        if (result == 0 || result == -1)
        {
            break;
        }
        size++;
    }
    string str = StringFromLength(allocator, size);
    fseek(fs, currentPos, SEEK_SET);
    fread(str.buffer, 1, size, fs);
    return str;
}
inline void FILE_PassString(void *self)
{
    FILE *fs = (FILE *)self;

    size_t size = 0;
    while (true)
    {
        int32_t result = fgetc(fs);
        if (result == 0 || result == -1)
        {
            break;
        }
    }
}
inline size_t FILE_GetCurrPos(void *self)
{
    FILE *fs = (FILE *)self;
    return (size_t)ftell(fs);
}
inline IDataStream GetFileDataStream(FILE *fs)
{
    IDataStream result;
    result.instance = fs;
    result.readFunc = &FILE_Read;
    result.jumpFunc = &FILE_Jump;
    result.getCurrPosFunc = &FILE_GetCurrPos;
    result.readStringFunc = &FILE_ReadString;
    return result;
}