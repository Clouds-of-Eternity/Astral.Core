#pragma once
#include "io.hpp"

#if WINDOWS
#include "Shlwapi.h"
#endif

namespace io
{
    inline bool PathMatches(const char *path, const char *pattern)
    {
#if WINDOWS
        return PathMatchSpec(path, pattern);
#else
        return fnmatch(pattern, path, 0);
#endif
    }

    inline u32 FindFilesByNamePattern(IAllocator allocator, const char *dirPath, const char *pattern, collections::List<string> *output, bool recursive)
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
            if (recursive)
            {
                OutputFoldersInDirectory(tempAllocator, folder.buffer, &foldersToProcess);
            }
        }
        u32 added = 0;
        for (u32 i = 0; i < results.count; i++)
        {
            if (PathMatches(results[i].buffer, pattern))
            {
                added++;
                output->Add(results[i]);
            }
            else
            {
                results[i].deinit();
            }
        }
        return added;
    }

    inline bool FileContainsText(const char *filePath, const char *containingText, char *tempReadBlock, usize tempReadBlockSize)
    {
        //char readBlock[256];
        bool created = tempReadBlock == NULL;
        if (created)
        {
            if (tempReadBlockSize == 0)
            {
                tempReadBlockSize = 1024;
            }
            tempReadBlock = (char *)DEFAULT_ALLOC(tempReadBlockSize - 1);
        }
        else
        {
            if (tempReadBlockSize == 0)
            {
                return false;
            }
        }

        bool result = false;
        FILE *fs = fopen(filePath, "r");
        if (fs != NULL)
        {
            do
            {
                usize advance = fread(tempReadBlock, 1, tempReadBlockSize - 1, fs);
                
                tempReadBlock[advance] = '\0';
                if (strstr(tempReadBlock, containingText) != NULL)
                {
                    result = true;
                    break;
                }
            }
            while (!feof(fs));
            
            fclose(fs);
        }

        if (created)
        {
            DEFAULT_FREE(tempReadBlock);
        }
        return result;
    }
    inline u32 FindFilesContainingText(IAllocator outputAllocator, const char *dirPath, const char *pattern, const char *containingText, collections::List<string> *output, bool recursive)
    {
        i32 start = output->count;
        i32 resultsCount = FindFilesByNamePattern(outputAllocator, dirPath, pattern, output, recursive);

        char *readBlock = (char *)DEFAULT_ALLOC(1024);
        for (i32 i = (i32)output->count - 1; i >= start; i--)
        {
            if (i >= 0 && !FileContainsText(output->ptr[i].buffer, containingText, readBlock, 1024))
            {
                output->ptr[i].deinit();
                output->RemoveAt_Swap((usize)i);
            }
        }
        DEFAULT_FREE(readBlock);
        return output->count - start;
    }
}