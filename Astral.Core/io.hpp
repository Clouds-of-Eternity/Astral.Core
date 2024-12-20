#pragma once

#include "Linxc.h"
#include "string.hpp"
#include "array.hpp"
#include <stdio.h>
#include "vector.hpp"
#include "ArenaAllocator.hpp"

#include <sys/stat.h>   // For stat().

#if WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <Windows.h>
#define access _access
#endif
#if POSIX
#include <unistd.h>
#include <dirent.h>
#endif

namespace io
{
    inline string ReadFile(IAllocator allocator, const char* path, bool isBinary)
    {
        string result = string(allocator);

        FILE *fs = fopen(path, isBinary ? "rb" : "r");
        if (fs != NULL)
        {
            usize size = 0;
            while (fgetc(fs) != EOF)
            {
                size += 1;
            }
            fseek(fs, 0, SEEK_SET);

            char* buffer = (char*)allocator.Allocate(size + 1);
            if (buffer != NULL)
            {
                fread(buffer, sizeof(char), size, fs);
                
                buffer[size] = '\0';
                result.buffer = buffer;
                result.length = size + 1;
            }

            fclose(fs);
        }
        return result;
    }

    inline bool FileExists(const char *path)
    {
        return access(path, 0) == 0;
    }

    inline bool DirectoryExists(const char* path)
    {
        if (access(path, 0) == 0) 
        {
            struct stat status;

            stat(path, &status);

            return (status.st_mode & S_IFDIR) != 0;
        }
        return false;
    }

    inline bool NewDirectory(const char* path)
    {
        if (!io::DirectoryExists(path))
        {
        #if WINDOWS
            return CreateDirectoryA(path, NULL);
        #else
            return mkdir(path, 0755) == 0;
        #endif

        }
        return false;
    }

    inline void RecursiveCreateDirectories(const char* finalDirPath)
    {
        ArenaAllocator arena = ArenaAllocator(GetCAllocator());
        IAllocator alloc = arena.AsAllocator();

        collections::Array<string> paths = SplitString(alloc, finalDirPath, '/');
        if (paths.length <= 1) //C:/ is not a valid file
        {
            return;
        }
        string currentPath = paths.data[0].Clone(alloc);

        for (usize i = 0; i < paths.length; i++)
        {
            if (i > 0)
            {
                currentPath.Append("/");
                currentPath.Append(paths.data[i].buffer);
            }
            if (!io::DirectoryExists(currentPath.buffer))
            {
                io::NewDirectory(currentPath.buffer);
            }
        }

        arena.deinit();
    }

    inline FILE* CreateDirectoriesAndFile(const char* path)
    {
        ArenaAllocator arena = ArenaAllocator(GetCAllocator());
        IAllocator alloc = arena.AsAllocator();
        collections::Array<string> paths = SplitString(alloc, path, '/');
        if (paths.length <= 1) //C:/ is not a valid file
        {
            return NULL;
        }
        FILE* file = NULL;
        string currentPath = paths.data[0].Clone(alloc);
        for (usize i = 0; i < paths.length; i++)
        {
            if (i > 0)
            {
                currentPath.Append("/");
                currentPath.Append(paths.data[i].buffer);
            }
            if (i < paths.length - 1)
            {
                if (!io::DirectoryExists(currentPath.buffer))
                {
                    io::NewDirectory(currentPath.buffer);
                }
            }
            else
            {
                //create file
                file = fopen(currentPath.buffer, "w");
                break;
            }
        }

        arena.deinit();
        return file;
    }

    inline collections::Array<string> GetFilesInDirectory(IAllocator allocator, const char *dirPath)
    {
        IAllocator defaultAllocator = GetCAllocator();

#if WINDOWS
        WIN32_FIND_DATAA findFileResult;
        char sPath[1024];
        sprintf(sPath, "%s/*.*", dirPath);

        HANDLE handle = FindFirstFileA(sPath, &findFileResult);
        if (handle == INVALID_HANDLE_VALUE)
        {
            return collections::Array<string>();
        }

        collections::vector<string> results = collections::vector<string>(defaultAllocator);
        while (true)
        {
            if (strcmp(findFileResult.cFileName, ".") != 0 && strcmp(findFileResult.cFileName, "..") != 0)
            {
                //printf("%s\n", &findFileResult.cFileName[0]);
                string replaced = ReplaceChar(defaultAllocator, &findFileResult.cFileName[0], '\\', '/');

                string fullPath = string(allocator, dirPath);
                fullPath.Append("/");
                fullPath.Append(replaced.buffer);
                if (!io::DirectoryExists(fullPath.buffer))
                {
                    results.Add(fullPath);
                }
                replaced.deinit();
            }
            if (!FindNextFileA(handle, &findFileResult))
            {
                break;
            }
        }

        FindClose(handle);

        return results.ToOwnedArrayWith(allocator);
#else
        collections::vector<string> results = collections::vector<string>(GetCAllocator());

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

            if (S_ISDIR(st.st_mode))
            {
                //dircount++
                string fullPath = string(allocator, dirPath);
                fullPath.Append("/");
                fullPath.Append(dent->d_name);
                results.Add(fullPath);
            }
        }

        return results.ToOwnedArrayWith(allocator);
#endif
    }

    inline collections::Array<string> GetFoldersInDirectory(IAllocator allocator, const char *dirPath)
    {
        IAllocator defaultAllocator = GetCAllocator();

#if WINDOWS
        WIN32_FIND_DATAA findFileResult;
        char sPath[1024];
        sprintf(sPath, "%s/*.*", dirPath);

        HANDLE handle = FindFirstFileA(sPath, &findFileResult);
        if (handle == INVALID_HANDLE_VALUE)
        {
            return collections::Array<string>();
        }

        collections::vector<string> results = collections::vector<string>(defaultAllocator);
        while (true)
        {
            if (strcmp(findFileResult.cFileName, ".") != 0 && strcmp(findFileResult.cFileName, "..") != 0)
            {
                //printf("%s\n", &findFileResult.cFileName[0]);
                string replaced = ReplaceChar(allocator, &findFileResult.cFileName[0], '\\', '/');

                string fullPath = string(defaultAllocator, dirPath);
                fullPath.Append("/");
                fullPath.Append(replaced.buffer);
                if (io::DirectoryExists(fullPath.buffer))
                {
                    results.Add(fullPath);
                }
                replaced.deinit();
            }
            if (!FindNextFileA(handle, &findFileResult))
            {
                break;
            }
        }

        FindClose(handle);

        return results.ToOwnedArrayWith(allocator);
#else
        collections::vector<string> results = collections::vector<string>(defaultAllocator);
        struct dirent *dir;
        DIR *d = opendir(dirPath);
        if (d != NULL) 
        {
            while ((dir = readdir(d)) != NULL) 
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
                    string fullPath = string(allocator, dirPath);
                    fullPath.Append("/");
                    fullPath.Append(dir->d_name);
                    results.Add(fullPath);
                }
            }
            closedir(d);
        }

        return results.ToOwnedArrayWith(allocator);
#endif
    }

    inline collections::Array<string> GetFilesInDirectoryRecursive(IAllocator allocator, const char* dirPath)
    {
        ArenaAllocator arena = ArenaAllocator(GetCAllocator());
        IAllocator alloc = arena.AsAllocator();
        collections::vector<string> results = collections::vector<string>(alloc);
        collections::vector<string> foldersToProcess = collections::vector<string>(alloc);
        foldersToProcess.Add(string(alloc, dirPath));

        while (foldersToProcess.count > 0)
        {
            string folder = foldersToProcess.ptr[0];
            foldersToProcess.RemoveAt_Swap(0);

            collections::Array<string> filesInThisDir = GetFilesInDirectory(alloc, folder.buffer);
            for (usize i = 0; i < filesInThisDir.length; i++)
            {
                results.Add(filesInThisDir[i].Clone(allocator));
            }
            collections::Array<string> foldersInThisDir = GetFoldersInDirectory(alloc, folder.buffer);
            for (usize i = 0; i < foldersInThisDir.length; i++)
            {
                foldersToProcess.Add(foldersInThisDir[i]);
            }
        }
        collections::Array<string> finalArray = results.ToClonedArray(allocator);
        arena.deinit();

        return finalArray;
    }
}