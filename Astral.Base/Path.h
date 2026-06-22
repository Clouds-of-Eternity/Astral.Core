#pragma once
#include "./StringPool.h"

string PathStrSwapExtension(IAllocator allocator, const string path, const char* newExtension)
{
    size_t dotPosition = path.length + 1; //impossible number
    size_t finalDirPosition = path.length + 1;
    size_t i = 0;

    while (path.buffer[i] != '\0')
    {
        if (path.buffer[i] == '.')
        {
            dotPosition = i;
        }
        else if (path.buffer[i] == '/' || path.buffer[i] == '\\')
        {
            finalDirPosition = i;
        }
        i++;
    }

    if (dotPosition != path.length + 1 && (finalDirPosition == path.length + 1 || finalDirPosition < dotPosition))
    {
        string result;
        if (newExtension != NULL)
        {
            size_t extLength = strlen(newExtension);
            result = StringFromLength(allocator, dotPosition + 1 + extLength);
            result.buffer[dotPosition] = '/';
            memcpy(&result.buffer[dotPosition + 1], newExtension, extLength);
        }
        else result = StringFromLength(allocator, dotPosition);
        memcpy(result.buffer, path.buffer, dotPosition);

        return result;
    }
    else
    {
        if (newExtension != NULL)
        {
            size_t extLength = strlen(newExtension);
            //dont need to +1 or -1 to length here since the accurate representation,
            //-1 to discount the null terminator and +1 to include the '/' cancels out
            string result = StringFromLength(allocator, path.length + extLength);
            memcpy(result.buffer, path.buffer, path.length - 1);
            result.buffer[path.length] = '/';
            memcpy(&result.buffer[path.length + 1], newExtension, extLength);

            return result;
        }
        else return StringClone(allocator, path);
    }
}
static inline string PathSwapExtension(IAllocator allocator, const char *path, const char* newExtension)
{
    string str = RENTSTR(path);

    string swap = PathStrSwapExtension(allocator, str, newExtension);

    RETSTR(str);

    return swap;
}

/// @brief Gets the extension of a given path string object, including the leading '.'
static inline string PathStrGetExtension(IAllocator allocator, string path)
{
    size_t dotPosition = path.length + 1; //impossible number
    size_t finalDirPosition = path.length + 1;
    size_t i = 0;

    while (path.buffer[i] != '\0')
    {
        if (path.buffer[i] == '.')
        {
            dotPosition = i;
        }
        else if (path.buffer[i] == '/' || path.buffer[i] == '\\')
        {
            finalDirPosition = i;
        }
        i++;
    }

    if (dotPosition != path.length + 1 && (finalDirPosition == path.length + 1 || finalDirPosition < dotPosition))
    {
        return StringFromSlice(allocator, path.buffer + dotPosition, path.length - dotPosition - 1);
    }
    else
    {
        return StringEmpty();
    }
}
/// @brief Gets the extension of a given path string literal, including the leading '.'
static inline string PathGetExtension(IAllocator allocator, const char *path)
{
    string str = RENTSTR(path);

    string ext = PathStrGetExtension(allocator, str);

    RETSTR(str);

    return ext;
}

static inline string PathStrGetDirectory(IAllocator allocator, string path)
{
    size_t actualLastIndex = 0;
    if (!StringFindAnyFromEnd(path, "/\\", &actualLastIndex))
    {
        return StringEmpty();
    }
    return StringFromSlice(allocator, path.buffer, actualLastIndex);
}
static inline string PathGetDirectory(IAllocator allocator, const char *path)
{
    string str = RENTSTR(path);

    string directory = PathStrGetDirectory(allocator, str);

    RETSTR(str);

    return directory;
}

static inline string PathStrGetFileName(IAllocator allocator, string path)
{
    size_t lastIndex;
    bool hasLast = StringFindAnyFromEnd(path, "/\\", &lastIndex);
    if (hasLast)
    {
        return StringFromSlice(allocator, path.buffer + lastIndex + 1, path.length - lastIndex - 1);
    }
    return StringClone(allocator, path);
}
static inline string PathGetFileName(IAllocator allocator, const char *path)
{
    string str = RENTSTR(path);

    string fileName = PathStrGetFileName(allocator, str);

    RETSTR(str);

    return fileName;
}