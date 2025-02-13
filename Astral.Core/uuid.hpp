#pragma once
#include "random.hpp"
#include "hash.hpp"
#include "allocators.hpp"
#include "string.hpp"

struct uuid
{
    u8 byte[16];

    inline uuid()
    {
        ((u64*)byte)[0] = 0;
        ((u64*)byte)[1] = 0;
    }
    inline static uuid New(Random* random)
    {
        uuid result;
        u64* asPointer = (u64*)&result;
        asPointer[0] = random->Next();
        asPointer[1] = random->Next();

        result.byte[6] = (result.byte[6] & 0x0f) | 0x40;
        result.byte[8] = (result.byte[8] & 0x3f) | 0x80;

        return result;
    }
    inline static uuid FromString(text text)
    {
        const u8 hexToNibble[] = {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        };
        const u8 positions[] = {0, 2, 4, 6, 9, 11, 14, 16, 19, 21, 24, 26, 28, 30, 32, 34};

        if (text[8] != '-' || text[13] != '-' || text[18] != '-' || text[23] != '-')
        {
            return uuid{};
        }

        uuid result = uuid{};
        for (usize i = 0; i < 16; i++)
        {
            const auto high = hexToNibble[text[positions[i]]];
            const auto low = hexToNibble[text[positions[i] + 1]];
            if (high == 0xff || low == 0xff)
            {
                return uuid{};
            }
            result.byte[i] = high << 4 | low;
        }
        return result;
    }

    inline bool operator==(uuid other)
    {
        u64* asPointer = (u64*)this;
        u64* otherAsPointer = (u64*)&other;
        return asPointer[0] == otherAsPointer[0] && asPointer[1] == otherAsPointer[1];
    }
    inline bool operator!=(uuid other)
    {
        u64* asPointer = (u64*)this;
        u64* otherAsPointer = (u64*)&other;
        return asPointer[0] != otherAsPointer[0] || asPointer[1] != otherAsPointer[1];
    }
    inline bool Equals(uuid other)
    {
        //lol
        u64* asPointer = (u64*)byte;
        u64* otherAsPointer = (u64*)other.byte;
        return asPointer[0] == otherAsPointer[0] && asPointer[1] == otherAsPointer[1];
    }
    inline u32 GetHashCode()
    {
        return Murmur3(byte, 16);
    }
    inline void GetAsString(char* buffer)
    {
        buffer[8] = '-';
        buffer[13] = '-';
        buffer[18] = '-';
        buffer[23] = '-';

        const text hex = "0123456789abcdef";
        const char positions[] = {0, 2, 4, 6, 9, 11, 14, 16, 19, 21, 24, 26, 28, 30, 32, 34};

        // for (usize i = 0; i < 16; i++)
        // {
        //     buffer[positions[i]] = hex[byte[i] >> 4];
        //     buffer[positions[i] + 1] = hex[byte[i] & 0xf];
        // }

//manual for loop unrolling go
#define SET_BUFFER(i) buffer[positions[i]] = hex[byte[i] >> 4]; buffer[positions[i] + 1] = hex[byte[i] & 0xf]

        SET_BUFFER(0);
        SET_BUFFER(1);
        SET_BUFFER(2);
        SET_BUFFER(3);
        SET_BUFFER(4);
        SET_BUFFER(5);
        SET_BUFFER(6);
        SET_BUFFER(7);
        SET_BUFFER(8);
        SET_BUFFER(9);
        SET_BUFFER(10);
        SET_BUFFER(11);
        SET_BUFFER(12);
        SET_BUFFER(13);
        SET_BUFFER(14);
        SET_BUFFER(15);

#undef SET_BUFFER
    }

    inline string ToString(IAllocator allocator)
    {
        char buffers[37];
        buffers[36] = '\0';
        GetAsString(buffers);
        return string(allocator, buffers);
    }
};
inline u32 UuidHash(uuid ID)
{
    return ID.GetHashCode();
}
inline bool UuidEql(uuid A, uuid B)
{
    return A.Equals(B);
}
