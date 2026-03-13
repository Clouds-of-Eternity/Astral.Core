#pragma once
#include "Common.h"

inline uint64_t rotl64(uint64_t value, uint64_t amount)
{
    amount %= 64;
    return (value << amount) | (value >> (64 - amount));
}
inline uint32_t rotl32(uint32_t value, uint32_t amount)
{
    amount %= 32;
    return (value << amount) | (value >> (32 - amount));
}
inline uint64_t rotr64(uint64_t value, uint64_t amount)
{
    amount %= 64;
    return (value >> amount) | (value << (64 - amount));
}
inline uint32_t rotr32(uint32_t value, uint32_t amount)
{
    amount %= 32;
    return (value >> amount) | (value << (32 - amount));
}