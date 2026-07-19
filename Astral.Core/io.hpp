#pragma once

#include "Linxc.h"
#include "string.hpp"
#include "Array.hpp"
#include <stdio.h>
#include "List.hpp"
#include "ArenaAllocator.hpp"
#include "Scope.hpp"
#include "Path.hpp"
#include "UTF8Utils.hpp"

#include <sys/stat.h>   // For stat().

#if WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <Windows.h>
#define access _access
#define stat _stat
#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & _S_IFDIR) != 0)
#endif
#endif
#if POSIX
#include <unistd.h>
#include <dirent.h>
#endif

namespace io
{
    inline long GetFileSize(FILE* ptr)
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

    inline string ReadFile(IAllocator allocator, const char* path, bool isBinary)
    {
        string result = string(allocator);
        FILE *fs = fopen(path, "rb");
        if (fs != NULL)
        {
            #ifdef WINDOWS
            if (!isBinary)
            {
                usize size = GetFileSize(fs);

                char* buffer = (char*)allocator.Allocate(size + 1);
                if (buffer != NULL)
                {
                    char chars[258];
                    //2 bytes of padding
                    chars[256] = '\0';
                    chars[257] = '\0';
                    usize index = 0;
                    usize actualSize = 0;
                    while (index < size)
                    {
                        //Read blocks of 256 characters at once.
                        //Since we have to iterate over the entire output of each read anyways, the
                        //main purpose of this is to reduce the number of file IO calls (in this case, fread)
                        //that must be done.
                        usize totalRead = fread(chars, sizeof(char), 256, fs);
                        actualSize += totalRead;
                        if (totalRead == 0)
                        {
                            break;
                        }
                        for (usize i = 0; i < totalRead; i++)
                        {
                            char curr = chars[i];
                            char next = chars[i + 1];

                            if (curr == '\r' && next == '\n')
                            {
                                buffer[index] = '\n';
                                i++;
                            }
                            else if (curr == '\r')
                            {
                                buffer[index] = '\n';
                            }
                            else
                            {
                                buffer[index] = curr;
                            }
                            index++;
                        }
                    }

                    //preprocessing
                    buffer[actualSize] = '\0';
                    result.buffer = buffer;
                    result.length = actualSize + 1;
                }

                fclose(fs);
            }
            else
            #endif
            {
                usize size = GetFileSize(fs);

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
        }
        return result;
    }

    inline bool FileExists(const char *path)
    {
        return access(path, 0) == 0;
    }

    inline bool DirectoryExists(const char* path)
    {
        struct stat status;

        if (stat(path, &status) != 0)
            return false;

        return S_ISDIR(status.st_mode);
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

    /// @brief Recursively creates all directories up to and including the provided absolute path
    /// @return True if the creation was successful, false if all directories already exist
    /// or if any directory had failed to create.
    inline bool RecursiveCreateDirectories(CharSlice finalDirPath)
    {
        if (finalDirPath.length <= 1)
        {
            return false;
        }
        bool result = true;
        char *chars = (char *)DEFAULT_ALLOC(finalDirPath.length + 1);
        chars[finalDirPath.length] = '\0';

        for (usize i = 0; i < finalDirPath.length; i++)
        {
            if (finalDirPath[i] == '\\' || finalDirPath[i] == '/')
            {
                chars[i] = '\0';
                if (!io::DirectoryExists(chars))
                {
                    result = io::NewDirectory(chars);
                    if (!result)
                    {
                        break;
                    }
                }
            }
            chars[i] = finalDirPath[i];
        }

        DEFAULT_FREE(chars);
        return result;
    }

    inline FILE* CreateDirectoriesAndFile(CharSlice absolutePath, bool writeBinary)
    {
        string dirPath = path::GetDirectory(absolutePath).ToString(GetCAllocator());
        if (dirPath.length > 0 && !io::DirectoryExists(dirPath.buffer) && !RecursiveCreateDirectories(dirPath))
        {
            dirPath.deinit();
            return NULL;
        }
        char *chars = (char *)DEFAULT_ALLOC(absolutePath.length + 1);
        memcpy(chars, absolutePath.buffer, absolutePath.length);
        chars[absolutePath.length] = '\0';

        FILE *result = fopen(chars, writeBinary ? "wb" : "w");
        DEFAULT_FREE(chars);
        dirPath.deinit();
        return result;
    }

    inline u32 OutputFilesInDirectory(IAllocator allocator, const char *dirPath, collections::List<string> *output)
    {
        u32 resultCount = 0;
        ArenaAllocator arenaAlloc = ArenaAllocator(GetCAllocator());
        Scope(ArenaAllocator, arenaAlloc);

        IAllocator tempAllocator = arenaAlloc.AsAllocator();

#if WINDOWS
        WIN32_FIND_DATAA findFileResult;
        char sPath[256];
        sprintf(sPath, "%s/*.*", dirPath);

        HANDLE handle = FindFirstFileA(sPath, &findFileResult);
        if (handle == INVALID_HANDLE_VALUE)
        {
            return 0;
        }

        while (true)
        {
            if (strcmp(findFileResult.cFileName, ".") != 0 && strcmp(findFileResult.cFileName, "..") != 0)
            {
                //printf("%s\n", &findFileResult.cFileName[0]);
                string replaced = ReplaceChar(tempAllocator, &findFileResult.cFileName[0], '\\', '/');

                string fullPath = string::Format(tempAllocator, "%s/%s", dirPath, replaced.buffer);
                if (!io::DirectoryExists(fullPath.buffer))
                {
                    output->Add(fullPath.Clone(allocator));
                    resultCount++;
                }
            }
            if (!FindNextFileA(handle, &findFileResult))
            {
                break;
            }
        }

        FindClose(handle);

        return resultCount;
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
                resultCount++;
                output->Add(string::Format(allocator, "%s/%s", dirPath, dent->d_name));
            }
        }

        return resultCount;
#endif
    }
    /// @brief Retrieves an array of all files in a given directory.
    /// @param allocator The allocator to use
    /// @param dirPath The directory to check
    /// @return An array of full file paths
    inline collections::Array<string> GetFilesInDirectory(IAllocator allocator, const char *dirPath)
    {
        collections::List<string> paths = collections::List<string>(GetCAllocator());
        OutputFilesInDirectory(allocator, dirPath, &paths);
        return paths.ToOwnedArrayWith(allocator);
    }

    inline u32 OutputFoldersInDirectory(IAllocator allocator, const char *dirPath, collections::List<string> *output)
    {
        u32 resultCount = 0;

#if WINDOWS
        ArenaAllocator arenaAlloc = ArenaAllocator(GetCAllocator());
        Scope(ArenaAllocator, arenaAlloc);

        IAllocator tempAllocator = arenaAlloc.AsAllocator();
        
        WIN32_FIND_DATAA findFileResult;
        char sPath[256];
        sprintf(sPath, "%s/*.*", dirPath);

        HANDLE handle = FindFirstFileA(sPath, &findFileResult);
        if (handle == INVALID_HANDLE_VALUE)
        {
            return 0;
        }

        while (true)
        {
            if (strcmp(findFileResult.cFileName, ".") != 0 && strcmp(findFileResult.cFileName, "..") != 0)
            {
                //printf("%s\n", &findFileResult.cFileName[0]);
                string replaced = ReplaceChar(tempAllocator, &findFileResult.cFileName[0], '\\', '/');

                string fullPath = string::Format(tempAllocator, "%s/%s", dirPath, replaced.buffer);
                if (io::DirectoryExists(fullPath.buffer))
                {
                    resultCount++;
                    output->Add(fullPath.Clone(allocator));
                }
            }
            if (!FindNextFileA(handle, &findFileResult))
            {
                break;
            }
        }

        FindClose(handle);

        return resultCount;
#else
        struct dirent *dir;
        DIR *srcdir = opendir(dirPath);
        if (srcdir != NULL) 
        {
            while ((dir = readdir(srcdir)) != NULL) 
            {
                struct stat st;

                if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                {
                    continue;
                }
                if (fstatat(dirfd(srcdir), dir->d_name, &st, 0) < 0)
                {
                    continue;
                }

                if (S_ISDIR(st.st_mode))
                {
                    resultCount++;
                    output->Add(string::Format(allocator, "%s/%s", dirPath, dir->d_name));
                }
            }
            closedir(srcdir);
        }

        return resultCount;
#endif
    }
    /// @brief Retrieves an array of all sub-directories in a given directory.
    /// @param allocator The allocator to use
    /// @param dirPath The directory to check
    /// @return An array of full directory paths
    inline collections::Array<string> GetFoldersInDirectory(IAllocator allocator, const char *dirPath)
    {
        collections::List<string> paths = collections::List<string>(GetCAllocator());
        OutputFoldersInDirectory(allocator, dirPath, &paths);
        return paths.ToOwnedArrayWith(allocator);
    }

    /// @brief Gets all files within the given directory.
    /// @param allocator The allocator to use
    /// @param dirPath The directory to check
    /// @return An array of full file paths
    inline collections::Array<string> GetFilesInDirectoryRecursive(IAllocator allocator, const char* dirPath)
    {
        ArenaAllocator arena = ArenaAllocator(GetCAllocator());
        IAllocator tempAllocator = arena.AsAllocator();
        collections::List<string> results = collections::List<string>(tempAllocator, 32);
        collections::List<string> foldersToProcess = collections::List<string>(tempAllocator, 32);
        foldersToProcess.Add(string(tempAllocator, dirPath));

        while (foldersToProcess.count > 0)
        {
            string folder = foldersToProcess.Pop();

            OutputFilesInDirectory(allocator, folder.buffer, &results);
            OutputFoldersInDirectory(tempAllocator, folder.buffer, &foldersToProcess);
        }
        collections::Array<string> finalArray = results.ToClonedArray(allocator);
        arena.deinit();

        return finalArray;
    }
}