#pragma once
#include "Linxc.h"
#include "time.h"
#include "assert.h"
#include "Maths/Util.hpp"

struct SplitMix64
{
    u64 state;

    SplitMix64(u64 seed)
    {
        state = seed;
    }
    u64 Next()
    {
        state += 0x9e3779b97f4a7c15LLU;
        u64 z = state;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9LLU;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebLLU;
        return z ^ (z >> 31);
    }
};
struct Random
{
    u64 state0;
    u64 state1;
    u64 state2;
    u64 state3;
    static Random init(u64 seed)
    {
        Random result;
        result.Seed(seed);
        return result;
    }
    static Random FromTime(i32 offset = 0)
    {
        Random result;
        u64 currentTime = time(NULL) + offset;
        result.Seed(currentTime);
        return result;
    }
    void Seed(u64 seed)
    {
        SplitMix64 initial = SplitMix64(seed);
        state0 = initial.Next();
        state1 = initial.Next();
        state2 = initial.Next();
        state3 = initial.Next();
    }
    u64 Next()
    {
        u64 r = Maths::rotl64(state0 + state3, 23) + state0;
        u64 t = state1 << 17;
        state2 ^= state0;
        state3 ^= state1;
        state1 ^= state2;
        state0 ^= state3;
        state2 ^= t;
        state3 = Maths::rotl64(state3, 45);
        return r;
    }
    i32 NextRange(i32 minInclusive, i32 maxExclusive)
    {
        assert(minInclusive < maxExclusive);
        i32 diff = abs(maxExclusive - minInclusive);
        u64 next = Next() % diff;
        return minInclusive + next;
    }
    float NextFloat(float maxValue)
    {
        assert(maxValue > 0.0f);
        u64 next = Next();
        float result = next / ((float)U64Max / maxValue);
        return result;
    }
    float NextFloatRange(float minValue, float maxValue)
    {
        assert(minValue < maxValue);
        float diff = fabsf(maxValue - minValue);
        u64 next = Next();
        float result = next / ((float)U64Max / diff);
        return minValue + result;
    }
};