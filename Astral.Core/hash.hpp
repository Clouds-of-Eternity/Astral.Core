#pragma once

#include "Linxc.h"
#include "string.h"
#include "Maths/Util.hpp"

inline u32 GetHash(u8* buffer, usize len)
{
    u32 hash = 7;
    for (usize i = 0; i < len; i++)
    {
        hash = hash * 31 + buffer[i];
    }
    return hash;
}

inline u32 GetHash(const char* ptr)
{
    u32 hash = 7;
    usize i = 0;
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

template<typename T>
u32 PointerHash(T* ptr)
{
    return (usize)ptr;
}

template<typename T>
bool PointerEql(T* A, T* B)
{
    return A == B;
}

template<typename T>
u32 IntegerHash(T integer)
{
    return (u32)integer;
}

template<typename T>
bool IntegerEql(T A, T B)
{
    return A == B;
}

inline u32 CombineHash(u32 left, u32 right)
{
    return left ^ (right + 0x9e3779b9 + (left << 6) + (left >> 2));
}

inline u32 i32Hash(i32 value)
{
    return (u32)value;
}
inline bool i32Eql(i32 A, i32 B)
{
    return A == B;
}

inline bool IsLittleEndian()
{
    i32 n = 1;
    return *((u8*)&n) == 1;
}

inline u16 ByteSwapU16(u16 num)
{
    return ((num & 0xffu) >> 8) | (num << 8);
}
inline u32 ByteSwapU32(u32 num)
{
    return ((num & 0xff000000u) >> 24) | ((num & 0x00ff0000u) >> 8) | ((num & 0x0000ff00u) << 8) | (num << 24);
}
inline u64 ByteSwapU64(u64 num)
{
    num = (num & 0x00000000FFFFFFFFllu) << 32 | (num & 0xFFFFFFFF00000000llu) >> 32;
    num = (num & 0x0000FFFF0000FFFFllu) << 16 | (num & 0xFFFF0000FFFF0000llu) >> 16;
    num = (num & 0x00FF00FF00FF00FFllu) << 8  | (num & 0xFF00FF00FF00FF00llu) >> 8;
    return num;
}
inline u64 Murmur2Seeded(u8* ptr, u64 len, u64 seed)
{
    u64 m = 0xc6a4a7935bd1e995llu;
    u64 h1 = seed ^ (len * m);
    for (u32 i = 0; i < len / 8; i += 1)
    {
        u64 v = ((u64*)ptr)[i];
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
    u64 rest = len & 7;
    u64 offset = len - rest;
    if (rest > 0)
    {
        u64 k1 = 0;
        memcpy(&k1, ptr + offset, rest);
        //CopyMemory(ptr + offset, cast(u8*)&k1, rest);
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
inline u64 Murmur2(u8* ptr, u64 len)
{
    return Murmur2Seeded(ptr, len, 0xc70f6907llu);
}
inline u32 Murmur3Seeded(u8* ptr, u64 len, u32 seed)
{
    const u32 c1 = 0xcc9e2d51;
    const u32 c2 = 0x1b873593;
    u32 h1 = seed;
    for (u32 i = 0; i < (len >> 2); i++) //divide by 4
    {
        u32 v = ((u32*)ptr)[i];
        u32 k1 = v;
        if (!IsLittleEndian())
        {
            v = ByteSwapU32(v);
        }
        k1 *= c1;
        k1 = Maths::rotl32(k1, 15);
        k1 *= c2;
        h1 ^= k1;
        h1 = Maths::rotl32(h1, 13);
        h1 *= 5;
        h1 += 0xe6546b64;
    }
    {
        u32 k1 = 0;
        const u32 offset = len & 0xfffffffc;
        const u32 rest = len & 3;
        if (rest == 3) {
            k1 ^= *((u32*)&ptr[offset + 2]) << 16;
        }
        if (rest >= 2) {
            k1 ^= *((u32*)&ptr[offset + 1]) << 8;
        }
        if (rest >= 1) {
            k1 ^= *((u32 *)&ptr[offset]);
            k1 *= c1;
            k1 = Maths::rotl32(k1, 15);
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
inline u32 Murmur3(u8* ptr, u64 len)
{
    return Murmur3Seeded(ptr, len, 0xc70f6907);
}