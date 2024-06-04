#define FORCE_NO_INTRINSICS

#ifndef FORCE_NO_INTRINSICS
#if defined(__AVX2__)
#   include <immintrin.h>
#elif defined(__AVX__)
#   include <immintrin.h>
#elif defined(__SSE4_2__)
#   include <nmmintrin.h>
#elif defined(__SSE4_1__)
#   include <smmintrin.h>
#elif defined(__SSSE3__)
#   include <tmmintrin.h>
#elif defined(__SSE3__)
#   include<pmmintrin.h>
#elif defined(__SSE2__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86_FP)
#   include<emmintrin.h>
#endif

#ifdef _M_AMD64
#endif

#if defined(__SSE4_2__) || defined(__SSE4_1__) || defined(__SSE3__) || defined(__SSE2__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86_FP)
#define USE_SSE
#endif

#endif
