#pragma once
#ifndef FORCE_NO_INTRINSICS

#ifdef x86_64
#include <immintrin.h>
#ifndef USE_SSE
#define USE_SSE
#endif

#endif

#endif
