#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "./Hash.h"
#include "./Random.h"
#include "./Strings.h"

#define UUID_STR_LEN 37

def_delegate(RandomNextU64, uint64_t);

typedef struct Uuid
{
    uint64_t num1;
    uint64_t num2;
} Uuid;

inline Uuid Uuid_Empty()
{
    const Uuid result = {};
    return result;
}
inline Uuid Uuid_FromU64s(uint64_t num1, uint64_t num2)
{
    const Uuid result = {num1, num2};
    return result;
}
inline Uuid Uuid_FromRandom(Random *randomInstance)
{
    Uuid result;
    uint8_t *bytes = (uint8_t *)&result.num1;
    result.num1 = Random_Next(randomInstance);
    result.num2 = Random_Next(randomInstance);

    bytes[6] = (bytes[6] & 0x0f) | 0x40;
    bytes[8] = (bytes[8] & 0x3f) | 0x80;

    return result;
}
inline Uuid Uuid_FromRandomFunction(RandomNextU64 randomNextFunction)
{
    Uuid result;
    uint8_t *bytes = (uint8_t *)&result.num1;
    result.num1 = randomNextFunction();
    result.num2 = randomNextFunction();

    bytes[6] = (bytes[6] & 0x0f) | 0x40;
    bytes[8] = (bytes[8] & 0x3f) | 0x80;

    return result;
}
inline Uuid Uuid_FromRandomString(const char *stringInput)
{
    uint32_t seed = Murmur3((const uint8_t *)stringInput, strlen(stringInput));
    Random tempRand = Random_FromSeed(seed);
    return Uuid_FromRandom(&tempRand);
}
inline Uuid NewFromStringHashFunction(const char *stringInput, uint32_t(customHashFunction)(const uint8_t *, size_t))
{
    uint32_t seed = customHashFunction((const uint8_t *)stringInput, strlen(stringInput));
    Random tempRand = Random_FromSeed(seed);
    return Uuid_FromRandom(&tempRand);
}
inline Uuid FromString(const char *text)
{
    const uint8_t hexToNibble[] = {
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0x00,
        0x01,
        0x02,
        0x03,
        0x04,
        0x05,
        0x06,
        0x07,
        0x08,
        0x09,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0x0a,
        0x0b,
        0x0c,
        0x0d,
        0x0e,
        0x0f,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0x0a,
        0x0b,
        0x0c,
        0x0d,
        0x0e,
        0x0f,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
    };
    const uint8_t positions[] = {0, 2, 4, 6, 9, 11, 14, 16, 19, 21, 24, 26, 28, 30, 32, 34};

    if (text[8] != '-' || text[13] != '-' || text[18] != '-' || text[23] != '-')
    {
        return Uuid_Empty();
    }

    Uuid result = Uuid_Empty();
    uint8_t *bytes = (uint8_t *)&result.num1;
    for (size_t i = 0; i < 16; i++)
    {
        const uint8_t high = hexToNibble[text[positions[i]]];
        const uint8_t low = hexToNibble[text[positions[i] + 1]];
        if (high == 0xff || low == 0xff)
        {
            return Uuid_Empty();
        }
        bytes[i] = high << 4 | low;
    }
    return result;
}
inline uint32_t UuidHash(Uuid self)
{
    return Murmur3((uint8_t *)&self, sizeof(Uuid));
}
inline bool UuidEqls(Uuid A, Uuid B)
{
    return A.num1 == B.num1 && A.num2 == B.num2;
}

inline uint32_t UuidPtr_Hash(const void *self)
{
    return Murmur3((uint8_t *)self, sizeof(Uuid));
}
inline bool UuidPtr_Eqls(const void *ID1, const void *ID2)
{
    const Uuid *A = (Uuid *)ID1;
    const Uuid *B = (Uuid *)ID2;
    return A->num1 == B->num1 && A->num2 == B->num2;
}

inline void UuidGetAsString(Uuid self, char *buffer)
{
    buffer[8] = '-';
    buffer[13] = '-';
    buffer[18] = '-';
    buffer[23] = '-';

    const char *hex = "0123456789abcdef";
    const char positions[] = {0, 2, 4, 6, 9, 11, 14, 16, 19, 21, 24, 26, 28, 30, 32, 34};

    const uint8_t *byte = (const uint8_t *)&self.num1;

    // for (usize i = 0; i < 16; i++)
    // {
    //     buffer[positions[i]] = hex[byte[i] >> 4];
    //     buffer[positions[i] + 1] = hex[byte[i] & 0xf];
    // }

// manual for loop unrolling go
#define SET_BUFFER(i)                         \
    buffer[positions[i]] = hex[byte[i] >> 4]; \
    buffer[positions[i] + 1] = hex[byte[i] & 0xf]

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
inline string UuidToString(Uuid self, IAllocator allocator)
{
    char buffers[UUID_STR_LEN];
    buffers[UUID_STR_LEN - 1] = '\0';
    UuidGetAsString(self, buffers);
    return StringFrom(allocator, buffers);
}

#define UUID_STRINGIFY(uuidVarName, resultVarName) \
    char resultVarName[UUID_STR_LEN];              \
    resultVarName[UUID_STR_LEN - 1] = 0;           \
    UuidGetAsString(uuidVarName, resultVarName);
#define UUID_ISEMPTY(uuidVarName) UuidEqls(uuidVarName, Uuid_Empty())