#pragma once

#include <stddef.h>
#include <stdint.h>

#ifndef NULL
#define NULL 0
#endif

#define def_delegate(name, returns, ...) typedef returns (*name)(__VA_ARGS__)
#define BeginExports()
#define EndExport()
#define suppressGC

#ifdef WINDOWS
#define exportDynamic __declspec(dllexport)
#define importDynamic __declspec(dllimport)
#else
#define exportDynamic __attribute__((visibility("default")))
#define importDynamic
#endif

#ifdef EXPORT_DYNAMIC_LIBRARY
#define DynamicFunction exportDynamic
#else
#define DynamicFunction importDynamic
#endif