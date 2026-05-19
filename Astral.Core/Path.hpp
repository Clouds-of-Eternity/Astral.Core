#pragma once

#include "Linxc.h"
#include "string.hpp"
#include "Array.hpp"
#include "option.hpp"
#include <math.h>

namespace path
{
    /// @brief Swaps the extension of a file path. newExtension must include the period
    inline string SwapExtension(IAllocator allocator, CharSlice path, CharSlice newExtension)
    {
        usize dotPosition = path.length + 1; //impossible number
        usize finalDirPosition = path.length + 1;

        for (u32 i = 0; i < path.length; i++)
        {
            if (path.buffer[i] == '.')
            {
                dotPosition = i;
            }
            else if (path.buffer[i] == '/' || path.buffer[i] == '\\')
            {
                finalDirPosition = i;
            }
        }

        if (dotPosition != path.length + 1 && (finalDirPosition == path.length + 1 || finalDirPosition < dotPosition))
        {
            string result = string(allocator, dotPosition + newExtension.length + 1);
            memcpy(result.buffer, path.buffer, dotPosition);
            if (newExtension != NULL)
            {
                memcpy(result.buffer + dotPosition, newExtension.buffer, newExtension.length);
            }
            result.buffer[result.length - 1] = '\0';
            return result;
        }
        else
        {
            string result = string(allocator, path.length + newExtension.length + 1);
            memcpy(result.buffer, path.buffer, path.length);
            if (newExtension != NULL)
            {
                memcpy(result.buffer + path.length, newExtension.buffer, newExtension.length);
            }
            result.buffer[result.length - 1] = '\0';
            return result;
        }
    }
    /// @brief Swaps the extension of a file path and deinits the original instance. newExtension must include the period
    inline string SwapExtensionDeinit(IAllocator allocator, string path, const char* newExtension)
    {
        string result = SwapExtension(allocator, path, newExtension);
        path.deinit();
        return result;
    }
    /// @brief Swaps the extension of a file path and deinits the original instance, while using the original's allocator to
    /// create the results. newExtension must include the period
    inline string SwapExtensionDeinit(string path, const char* newExtension)
    {
        string result = SwapExtension(path.allocator, path, newExtension);
        path.deinit();
        return result;
    }
    
    /// @brief Retrieves the extension as a char slice, includes the '.'
    inline CharSlice GetExtension(CharSlice path)
    {
        usize dotPosition = path.length + 1; //impossible number
        usize finalDirPosition = path.length + 1;

        for (u32 i = 0; i < path.length; i++)
        {
            if (path.buffer[i] == '.')
            {
                dotPosition = i;
            }
            else if (path.buffer[i] == '/' || path.buffer[i] == '\\')
            {
                finalDirPosition = i;
            }
        }

        if (dotPosition != path.length + 1 && (finalDirPosition == path.length + 1 || finalDirPosition < dotPosition))
        {
            return CharSlice(path.buffer + dotPosition, path.length - dotPosition);
        }
        else
        {
            return CharSlice();
        }
    }
    /// @brief Retrieves the extension and stores it in a string, includes the '.'
    inline string GetExtension(IAllocator allocator, CharSlice path)
    {
        CharSlice result = GetExtension(path);
        return string(allocator, result.buffer, result.length);
    }

    inline CharSlice GetDirectory(CharSlice path)
    {
        u32 lastSeparatorIndex = 0;
        for (u32 i = 0; i < path.length; i++)
        {
            if (path.buffer[i] == '/' || path.buffer[i] == '\\')
            {
                lastSeparatorIndex = i;
            }
        }
        return CharSlice(path.buffer, lastSeparatorIndex);
    }
    inline string GetDirectory(IAllocator allocator, CharSlice path)
    {
        CharSlice result = GetDirectory(path);
        return string(allocator, result.buffer, result.length);
    }
    inline string GetDirectoryDeinit(IAllocator allocator, string path)
    {
        string result = GetDirectory(allocator, path);
        path.deinit();
        return result;
    }
    inline string GetDirectoryDeinit(string path)
    {
        string result = GetDirectory(path.allocator, path);
        path.deinit();
        return result;
    }

    inline CharSlice GetFileName(CharSlice path)
    {
        u32 actualLastIndex = 0;
        for (u32 i = 0; i < path.length; i++)
        {
            if (path[i] == '/' || path[i] == '\\')
            {
                actualLastIndex = i + 1;
            }
        }

        return CharSlice(path.buffer + actualLastIndex, path.length - actualLastIndex);
    }
    inline string GetFileName(IAllocator allocator, CharSlice path)
    {
        CharSlice result = GetFileName(path);
        return string(allocator, result.buffer, result.length);
    }
    inline string GetFileNameDeinit(IAllocator allocator, string path)
    {
        string result = GetFileName(allocator, path);
        path.deinit();
        return result;
    }
}