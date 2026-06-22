#pragma once
#include "Bitwise.h"
#include "time.h"
#include "assert.h"
#include <math.h>

typedef struct SplitMix64
{
    uint64_t state;
} SplitMix64;

static inline uint64_t SplitMix64_Next(SplitMix64 *self)
{
    self->state += 0x9e3779b97f4a7c15LLU;
    uint64_t z = self->state;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9LLU;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebLLU;
    return z ^ (z >> 31);
}

typedef struct Random
{
    uint64_t state0;
    uint64_t state1;
    uint64_t state2;
    uint64_t state3;
} Random;

static inline void Random_SetSeed(Random *self, uint64_t seed)
{
    SplitMix64 initial = {seed};
    self->state0 = SplitMix64_Next(&initial);
    self->state1 = SplitMix64_Next(&initial);
    self->state2 = SplitMix64_Next(&initial);
    self->state3 = SplitMix64_Next(&initial);
}
static inline Random Random_FromSeed(uint64_t seed)
{
    Random result;
    Random_SetSeed(&result, seed);
    return result;
}
static inline Random Random_FromTime(int32_t offset)
{
    Random result;
    uint64_t currentTime = time(NULL) + offset;
    Random_SetSeed(&result, currentTime);
    return result;
}
static inline uint64_t Random_Next(Random *self)
{
    const uint64_t r = rotl64(self->state0 + self->state3, 23) + self->state0;
    const uint64_t t = self->state1 << 17;
    self->state2 ^= self->state0;
    self->state3 ^= self->state1;
    self->state1 ^= self->state2;
    self->state0 ^= self->state3;
    self->state2 ^= t;
    self->state3 = rotl64(self->state3, 45);
    return r;
}
static inline int32_t Random_NextRange(Random *self, int32_t maxExclusive, int32_t minInclusive)
{
    const int32_t diff = abs(maxExclusive - minInclusive);
    const uint64_t next = Random_Next(self) % diff;
    return minInclusive + next;
}
static inline float Random_NextFloat(Random *self, float maxValue)
{
    const uint64_t next = Random_Next(self);
    const float result = next / ((float)0xffffffffffffffff / maxValue);
    return result;
}
static inline float Random_NextFloatRange(Random *self, float minValue, float maxValue)
{
    const float diff = fabsf(maxValue - minValue);
    const uint64_t next = Random_Next(self);
    const float result = next / ((float)0xffffffffffffffff / diff);
    return minValue + result;
}