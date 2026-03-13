#pragma once
#include "Bitwise.h"

inline uint32_t BufferHash(uint8_t* buffer, size_t len)
{
    uint32_t hash = 7;
    for (size_t i = 0; i < len; i++)
    {
        hash = hash * 31 + buffer[i];
    }
    return hash;
}

inline uint32_t StrHash(const char* ptr)
{
    uint32_t hash = 7;
    size_t i = 0;
    while (true)
    {
        if (ptr[i] == '\0')
        {
            break;
        }
        else
        {
            hash = hash * 31 + ptr[i];
        }
        i += 1;
    }
    return hash;
}

inline uint32_t CombineHash(uint32_t left, uint32_t right)
{
    return left ^ (right + 0x9e3779b9 + (left << 6) + (left >> 2));
}
inline bool IsLittleEndian()
{
    int32_t n = 1;
    return *((uint8_t*)&n) == 1;
}

inline uint16_t ByteSwapU16(uint16_t num)
{
    return ((num & 0xffu) >> 8) | (num << 8);
}
inline uint32_t ByteSwapU32(uint32_t num)
{
    return ((num & 0xff000000u) >> 24) | ((num & 0x00ff0000u) >> 8) | ((num & 0x0000ff00u) << 8) | (num << 24);
}
inline uint64_t ByteSwapU64(uint64_t num)
{
    num = (num & 0x00000000FFFFFFFFllu) << 32 | (num & 0xFFFFFFFF00000000llu) >> 32;
    num = (num & 0x0000FFFF0000FFFFllu) << 16 | (num & 0xFFFF0000FFFF0000llu) >> 16;
    num = (num & 0x00FF00FF00FF00FFllu) << 8  | (num & 0xFF00FF00FF00FF00llu) >> 8;
    return num;
}
inline uint64_t Murmur2Seeded(const uint8_t* ptr, uint64_t len, uint64_t seed)
{
    uint64_t m = 0xc6a4a7935bd1e995llu;
    uint64_t h1 = seed ^ (len * m);
    for (uint32_t i = 0; i < len / 8; i += 1)
    {
        uint64_t v = ((uint64_t*)ptr)[i];
        if (!IsLittleEndian())
        {
            v = ByteSwapU64(v);
        }
        v *= m;
        v ^= v >> 47;
        v *= m;
        h1 ^= v;
        h1 *= m;
    }
    uint64_t rest = len & 7;
    uint64_t offset = len - rest;
    if (rest > 0)
    {
        uint64_t k1 = 0;
        memcpy(&k1, ptr + offset, rest);
        //CopyMemory(ptr + offset, cast(uint8_t*)&k1, rest);
        if (!IsLittleEndian())
        {
            k1 = ByteSwapU64(k1);
        }
        h1 ^= k1;
        h1 *= m;
    }
    h1 ^= h1 >> 47;
    h1 *= m;
    h1 ^= h1 >> 47;
    return h1;
}
inline uint64_t Murmur2(const uint8_t* ptr, uint64_t len)
{
    return Murmur2Seeded(ptr, len, 0xc70f6907llu);
}
inline uint32_t Murmur3Seeded(const uint8_t* ptr, uint64_t len, uint32_t seed)
{
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    uint32_t h1 = seed;
    for (uint32_t i = 0; i < (len >> 2); i++) //divide by 4
    {
        uint32_t v = ((uint32_t*)ptr)[i];
        uint32_t k1 = v;
        if (!IsLittleEndian())
        {
            v = ByteSwapU32(v);
        }
        k1 *= c1;
        k1 = rotl32(k1, 15);
        k1 *= c2;
        h1 ^= k1;
        h1 = rotl32(h1, 13);
        h1 *= 5;
        h1 += 0xe6546b64;
    }
    {
        uint32_t k1 = 0;
        const uint32_t offset = len & 0xfffffffc;
        const uint32_t rest = len & 3;
        if (rest == 3) {
            k1 ^= (uint32_t)(ptr[offset + 2]) << 16;
        }
        if (rest >= 2) {
            k1 ^= (uint32_t)(ptr[offset + 1]) << 8;
        }
        if (rest >= 1) {
            k1 ^= (uint32_t)(ptr[offset]);
            k1 *= c1;
            k1 = rotl32(k1, 15);
            k1 *= c2;
            h1 ^= k1;
        }
    }
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;
    return h1;
}
inline uint32_t Murmur3(const uint8_t* ptr, uint64_t len)
{
    return Murmur3Seeded(ptr, len, 0xc70f6907);
}