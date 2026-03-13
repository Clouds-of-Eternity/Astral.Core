#pragma once
#include <string.h>
#include "allocators.hpp"

template <typename T>
inline bool CheckBitFlag(const T *ptr, usize ptrElementCount, u32 bitIndex)
{
    static_assert(sizeof(T) <= 8, "template parameter type must have size less than or equal to 8 bytes");
    const u32 sizeTBits = sizeof(T) * 8;
    const u32 ptrIndex = bitIndex / sizeTBits;
    const u32 bitOffset = bitIndex % sizeTBits;

    if (ptrIndex >= ptrElementCount)
    {
        return false;
    }
    return (ptr[ptrIndex] & ((T)1 << bitOffset)) != 0;
};
template <typename T>
inline bool SetBitFlag(T *ptr, usize ptrElementCount, bool value, u32 bitIndex)
{
    static_assert(sizeof(T) <= 8, "template parameter type must have size less than or equal to 8 bytes");
    const u32 sizeTBits = (u32)(sizeof(T) * 8);
    const u32 ptrIndex = bitIndex / sizeTBits;
    const u32 bitOffset = bitIndex % sizeTBits;

    if (ptrIndex >= ptrElementCount)
    {
        return false;
    }
    T flag = ((T)1 << bitOffset);
    //clear bit
    ptr[ptrIndex] = ptr[ptrIndex] & (~flag);
    //if value is true, toggle bit
    if (value)
    {
        ptr[ptrIndex] = ptr[ptrIndex] | flag;
    }

    return true;
};
/*struct BitFlagsDynamic
{
    IAllocator allocator;

    u64 *values;
    usize capacity;

    inline BitFlagsDynamic()
    {
        allocator = {};
        values = NULL;
        capacity = 0;
    }
    inline BitFlagsDynamic(IAllocator alloc)
    {
        allocator = alloc;
        values = NULL;
        capacity = 0;
    }
    inline bool Get(u32 index)
    {
        usize capacityBits = capacity * 64;
        if (index >= capacityBits)
        {
            return false;
        }
        u32 j = index / capacityBits;
        u32 i = index % capacityBits;
        return (values[j] & (1llu << i)) != 0;
    }
    inline void Set(bool newFlagValue, u32 index)
    {
        usize capacityBits = capacity * 64;
        if (index >= capacityBits)
        {
            usize oldCapacity = capacity;
            while (capacityBits <= index)
            {
                capacity *= 2;
                capacityBits = capacity * 64;
            }
            u64 *newValues = (u64 *)allocator.Allocate(capacity * sizeof(u64));
            memcpy(newValues, values, oldCapacity * sizeof(u64));
            memset(&newValues[oldCapacity], 0, sizeof(u64) * (capacity - oldCapacity));

            allocator.Free(values);
            values = newValues;
        }
        u32 j = index / capacityBits;
        u32 i = index % capacityBits;

        values[j] = values[j] & ~(1llu << i);
        //re-enable bit if necessary
        if (newFlagValue)
        {
            values[j] = values[j] | (1llu << i);
        }
    }
};
struct BitFlags64
{
    //64 bits = max 64 bits, or max 8 by 8
    u64 value;

    inline BitFlags64()
    {
        value = 0;
    }
    inline BitFlags64(u64 initialValue)
    {
        value = initialValue;
    }

    inline bool Get(u32 index)
    {
        return (value & (1llu << index)) != 0;
    }
    inline void Set(bool newFlagValue, u32 index)
    {
        value = value & ~(1llu << index);
        //re-enable bit if necessary
        if (newFlagValue)
        {
            value = value | (1llu << index);
        }
    }
    inline bool Get(u32 x, u32 y)
    {
        u32 index = x + y * 8;
        return (value & (1llu << index)) != 0;
    }
    inline void Set(bool newFlagValue, u32 x, u32 y)
    {
        u32 index = x + y * 8;
        //disable bit
        value = value & ~(1llu << index);
        //re-enable bit if necessary
        if (newFlagValue)
        {
            value = value | (1llu << index);
        }
    }
};*/