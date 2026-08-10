#pragma once
#include "string.hpp"
#include "UTF8Utils.hpp"

struct CliCanvas
{
    IAllocator allocator;
    float worldSpaceX;
    float worldSpaceY;
    float worldSpaceWidth;
    float worldSpaceHeight;

    float resolution;

    i32 width;
    i32 height;

    u32 emptyChar;
    u32 *chars;
    inline usize GetCharArrayLength()
    {
        //+1 on the X axis to account for the nextline, and +1 after the end to acount for the
        //null terminator
        return sizeof(u32) * (width + 1) * (height) + 1;
    }
    inline CliCanvas()
    {
        allocator = {};
        resolution = 0.0f;

        worldSpaceX = 0.0f;
        worldSpaceY = 0.0f;
        worldSpaceWidth = 0.0f;
        worldSpaceHeight = 0.0f;
        width = 0;
        height = 0;

        chars = NULL;
        emptyChar = 0;
    }
    inline CliCanvas(IAllocator allocator, float worldSpaceBoundsX, float worldSpaceBoundsY, float worldSpaceBoundsWidth, float worldSpaceBoundsHeight, float resolution, char emptyChar)
    {
        this->allocator = allocator;
        
        worldSpaceX = worldSpaceBoundsX;
        worldSpaceY = worldSpaceBoundsY;
        worldSpaceWidth = worldSpaceBoundsWidth;
        worldSpaceHeight = worldSpaceBoundsHeight;
        this->resolution = resolution;

        width = floorf(worldSpaceWidth * resolution);
        height = floorf(worldSpaceHeight * resolution);
        this->emptyChar = emptyChar;

        Clear();
    }
    inline void Clear()
    {
        usize charsCount = GetCharArrayLength();
        chars = (u32 *)allocator.Allocate(sizeof(u32) * charsCount);
        memset(chars, emptyChar, sizeof(u32) * (charsCount - 1));
        for (u32 j = 0; j < height; j++)
        {
            chars[width + j * (width + 1)] = '\n';
        }
        chars[charsCount - 1] = '\0';
    }
    inline void Set(u32 x, u32 y, u32 value)
    {
        chars[x + y * (width + 1)] = value;
    }
    inline void Erase(u32 x, u32 y)
    {
        chars[x + y * (width + 1)] = emptyChar;
    }
    inline void deinit()
    {
        allocator.Free(chars);
    }
    inline string ToStringUTF8(IAllocator allocator)
    {
        usize arrayLength = GetCharArrayLength();
        u8 *bytes = (u8 *)allocator.Allocate(arrayLength * 4);
        usize totalLength = 0;
        for (u32 i = 0; i < arrayLength - 1; i++)
        {
            char outBytes[4];
            totalLength += CharPointToUTF8(chars[i], outBytes);
        }
        bytes[totalLength - 1] = '\0';
        string result = string();
        result.allocator = allocator;
        result.buffer = (char *)bytes;
        result.length = totalLength + 1;

        return result;
    }
    inline void PrintTo(FILE *fs)
    {
        for (u32 j = 0; j < height; j++)
        {
            char placeAt;
            for (u32 i = 0; i < width; i++)
            {
                u32 charIndex = i + j * (width + 1);
                placeAt = (char)chars[charIndex];
                fputc(placeAt, fs);
            }
            fputc('\n', fs);
        }
    }
    inline void Print()
    {
        PrintTo(stdout);
    }
};