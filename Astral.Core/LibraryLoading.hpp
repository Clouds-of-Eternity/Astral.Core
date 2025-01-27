#pragma once
#include "string.hpp"

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#define STR(s) L ## s
#define CH(c) L ## c
#define DIR_SEPARATOR L'\\'

#define string_compare wcscmp
#include <Windows.h>

inline void *LoadMod(const char_t *path)
{
    HMODULE h = LoadLibraryW(path);
    assert(h != NULL);
    return (void*)h;
}
inline void *GetFuncPointer(void *h, const char *name)
{
    void *f = GetProcAddress((HMODULE)h, name);
    return f;
}
inline void UnloadMod(void* library)
{
    FreeLibrary((HMODULE)library);
}
#else
#include <dlfcn.h>
#include <limits.h>
#define STR(s) s
#define CH(c) c
#define DIR_SEPARATOR '/'
#define MAX_PATH PATH_MAX

#define string_compare strcmp

inline void *LoadMod(const char_t *path)
{
    void *h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
    assert(h != NULL);
    return h;
}
inline void *GetFuncPointer(void *h, const char *name)
{
    void *f = dlsym(h, name);
    return f;
}
inline void UnloadMod(void* library)
{
    dlclose(library);
}

#endif