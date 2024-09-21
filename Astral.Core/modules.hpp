#pragma once

#ifdef WINDOWS

#include "windows.h"
#define LoadFunction(module, functionName) GetProcAddress((HMODULE)module, functionName)

inline void *LoadMod(const char *path)
{
    return LoadLibraryA(path);
}
inline void *LoadMod(const wchar_t *path)
{
    return LoadLibraryW(path);
}
inline void UnloadMod(void *instance)
{
    FreeLibrary((HMODULE)instance);
}

#else

#include "dlfcn.h"
#define LoadFunction(module, functionName) dlsym(module, functionName)

inline void *LoadMod(const char *path)
{
    return dlopen(path, RTLD_LAZY)
}
inline void UnloadMod(void *modHandle)
{
    dlclose(modHandle);
}

#endif