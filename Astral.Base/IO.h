#pragma once
#include "Strings.h"
#include "ArenaAllocator.h"
#include "Array.h"
#include <sys/stat.h>   // For stat().

#if WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <Windows.h>
#define access _access
#define stat _stat
#define S_ISDIR _S_IFDIR
#endif
#if POSIX
#include <unistd.h>
#include <dirent.h>
#endif

inline long IOGetFileSize(FILE* ptr)
{
    // TODO (Chris): Assert
    if (ptr == NULL)
    {
        return -1L;
    }

    long current_pos = ftell(ptr);

    if (fseek(ptr, 0, SEEK_END) != 0)
    {
        return -1L;
    }
    
    long size = ftell(ptr);
    if (size == -1L) 
    {
        return -1L;
    }
    
    // Set back old pos
    if (fseek(ptr, current_pos, SEEK_SET) != 0)
    {
        return -1L;
    }

    return size;
}

inline string IOReadFile(IAllocator allocator, const char* path, bool isBinary)
{
    string result = {allocator, NULL, 0};
    if (!isBinary)
    {
        FILE *fs = fopen(path, "r");
        if (fs != NULL)
        {
            size_t size = IOGetFileSize(fs);

            char* buffer = (char*)IAllocator_Allocate(allocator, size + 1);
            if (buffer != NULL)
            {
                fread(buffer, sizeof(char), size, fs);
                
                buffer[size] = '\0';
                result.buffer = buffer;
                result.length = size + 1;
            }

            fclose(fs);
        }
    }
    else
    {
        FILE *fs = fopen(path, "rb");
        if (fs != NULL)
        {
            size_t size = 0;
            fseek(fs, 0, SEEK_END);
            size = (size_t)ftell(fs);

            fseek(fs, 0, SEEK_SET);

            char* buffer = (char*)IAllocator_Allocate(allocator, size + 1);
            if (buffer != NULL)
            {
                fread(buffer, sizeof(char), size, fs);
                
                buffer[size] = '\0';
                result.buffer = buffer;
                result.length = size + 1;
            }

            fclose(fs);
        }
    }
    return result;
}
inline bool IOFileExists(const char *path)
{
    return access(path, 0) == 0;
}
inline bool IODirectoryExists(const char* path)
{
    if (access(path, 0) == 0) 
    {
        struct stat status;

        stat(path, &status);

        return (status.st_mode & S_ISDIR) != 0;
    }
    return false;
}

inline bool IONewDirectory(const char* path)
{
    if (!IODirectoryExists(path))
    {
    #if WINDOWS
        return CreateDirectoryA(path, NULL);
    #else
        return mkdir(path, 0755) == 0;
    #endif

    }
    return false;
}
inline void IORecursiveCreateDirectories(const char* finalDirPath)
{
    ArenaAllocator arena = ArenaAllocator_Create(GetCAllocator());
    IAllocator alloc = ArenaAllocator_AsAllocator(&arena);

    Array paths = SplitStringOnChar(alloc, finalDirPath, '/');
    if (paths.length <= 1) //C:/ is not a valid file
    {
        return;
    }
    string currentPath = ARRAY_AS(paths, string)[0];

    for (size_t i = 0; i < paths.length; i++)
    {
        if (i > 0)
        {
            currentPath = StringFormat(alloc, "%s/%s", currentPath.buffer, ARRAY_AS(paths, string)[i].buffer);
        }
        if (!IODirectoryExists(currentPath.buffer))
        {
            IONewDirectory(currentPath.buffer);
        }
    }

    ArenaAllocator_Deinit(&arena);
}

inline Array GetFilesInDirectory(IAllocator allocator, const char *dirPath)
{
    //this
    ArenaAllocator arenaAlloc = ArenaAllocator_Create(GetCAllocator());

    IAllocator tempAllocator = ArenaAllocator_AsAllocator(&arenaAlloc);
    List results = LIST(string, tempAllocator);

#if WINDOWS
    WIN32_FIND_DATAA findFileResult;
    char sPath[1024];
    sprintf(sPath, "%s/*.*", dirPath);

    HANDLE handle = FindFirstFileA(sPath, &findFileResult);
    if (handle == INVALID_HANDLE_VALUE)
    {
        ArenaAllocator_Deinit(&tempAllocator);
        return Array_Empty();
    }

    while (true)
    {
        if (strcmp(findFileResult.cFileName, ".") != 0 && strcmp(findFileResult.cFileName, "..") != 0)
        {
            string replaced = ReplaceChar(tempAllocator, &findFileResult.cFileName[0], '\\', '/');

            string fullPath = StringFormat(tempAllocator, "%s/%s", dirPath, replaced.buffer);
            if (!IODirectoryExists(fullPath.buffer))
            {
                List_Add(&results, &fullPath);
            }
        }
        if (!FindNextFileA(handle, &findFileResult))
        {
            break;
        }
    }

    FindClose(handle);
#else
    struct dirent *dent;
    DIR *srcdir = opendir(dirPath);
    while((dent = readdir(srcdir)) != NULL)
    {
        struct stat st;

        if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
        {
            continue;
        }
        if (fstatat(dirfd(srcdir), dent->d_name, &st, 0) < 0)
        {
            continue;
        }

        if (!S_ISDIR(st.st_mode))
        {
            string fullPath = StringFormat(allocator, "%s/%s", dirPath, dent->d_name);
            List_Add(&results, &fullPath);
        }
    }
#endif

    if (results.count == 0)
    {
        ArenaAllocator_Deinit(&tempAllocator);
        return Array_Empty();
    }

    Array resultArray = ARRAY(string, allocator, results.count);
    memcpy(resultArray.ptr, results.ptr, results.count * sizeof(string));

    ArenaAllocator_Deinit(&tempAllocator);
    return resultArray;
}