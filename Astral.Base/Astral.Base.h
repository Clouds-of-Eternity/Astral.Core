#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef NULL
#define NULL 0
#endif

#define def_delegate(name, returns, ...) typedef returns (*name)(__VA_ARGS__)
#define BeginExports()
#define EndExport()

#ifdef WINDOWS
#define exportDynamic __declspec(dllexport)
#define importDynamic __declspec(dllimport)
#else
#define exportDynamic __attribute__((visibility("default")))
#define importDynamic
#endif

#ifdef __cplusplus
#define CLinkage extern "C"
#else
#define CLinkage
#endif

#ifdef USE_DYNAMIC

#ifdef EXPORT_DYNAMIC_LIBRARY
#define DynamicFunction exportDynamic CLinkage
#else
#define DynamicFunction importDynamic CLinkage
#endif

#else
#define DynamicFunction CLinkage
#endif