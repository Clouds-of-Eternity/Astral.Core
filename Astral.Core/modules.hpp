#pragma once

#ifdef WINDOWS

#include "windows.h"
#define LoadFunction(module, functionName) GetProcAddress((HINSTANCE)module, functionName)

inline void *LoadMod(const char *path)
{
    return LoadLibraryA(path);
}
inline void *LoadMod(const wchar_t *path)
{
    return LoadLibraryW(path);
}

#else

#include "dlfcn.h"
#define symLoad dlsym

inline void *LoadMod(const char *path)
{
    return dlopen(path, RTLD_LAZY)
}

#endif