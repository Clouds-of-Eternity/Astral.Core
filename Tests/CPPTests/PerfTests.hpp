#pragma once

#include "Maths/Matrix4x4.hpp"
#include <stdio.h>
#include "./sokol_time.h"
#include "io.hpp"

#include "xmmintrin.h"
#include "Json.hpp"

#include <fstream>
#include <CPPTests/nlohmann.hpp>

static inline void ParseJson5MB()
{
    printf("Parsing JSON with Astral.Core\n");
    {
        ArenaAllocator arena = ArenaAllocator(GetCAllocator());

        u64 start = stm_now();
        string str = io::ReadFile(GetCAllocator(), "./json5mb.json", false);
        Json::JsonElement root = {};
        usize errorLine = Json::ParseJsonDocument(arena.AsAllocator(), str, &root);
        u64 elapsed = stm_since(start);
        double seconds = stm_sec(elapsed);

        printf("elapsed time: %llu ticks, %f seconds\n", elapsed, seconds);
        if (errorLine != 0)
        {
            printf(" - result: error on line %llu\n", errorLine);
        }
        else
        {
            printf("result: no error, root has %llu elements\n", root.arrayElements[0].value.arrayElements.length);
        }

        arena.deinit();
        str.deinit();
    }
    printf("Parsing JSON with Nlohmann\n");
    {
        std::ifstream f("./json5mb.json");
        u64 start = stm_now();
        nlohmann::json data = nlohmann::json::parse(f);
        u64 elapsed = stm_since(start);
        double seconds = stm_sec(elapsed);
        printf("elapsed time: %llu ticks, %f seconds\n", elapsed, seconds);
    }
}
static inline void FindInString()
{
    string str = io::ReadFile(GetCAllocator(), "./TestData.txt", false);

    assert(str.buffer != NULL);

    //no simd
    {
        u64 result = 0;
        u64 start = stm_now();
        for (u32 i = 0; i < str.length - 1; i++)
        {
            if (str[i] == ' ')
            {
                result++;
            }
        }
        u64 elapsed = stm_since(start);
        double seconds = stm_sec(elapsed);

        printf("elapsed time: %llu ticks, %f seconds\n", elapsed, seconds);
        printf(" - result: %llu spaces in %llu characters\n", result, str.length - 1);
    }
    //simd
    {
        u64 result = 0;
        u64 start = stm_now();
        u32 i = 0;
        const __m128i toSearch = _mm_set1_epi8(' ');
        while (i < str.length - 17)
        {
            const __m128i chars = _mm_loadu_si128((const __m128i_u *)(str.buffer + i));//_mm_load_epi8(str.buffer + i);
            const __m128i compared = _mm_cmpeq_epi8(chars, toSearch);
            int moved = _mm_movemask_epi8(compared);

            int total = _popcnt32(moved);
            result += total;
            i += 16;
        }
        for (; i < str.length - 1; i++)
        {
            if (str[i] == ' ')
            {
                result++;
            }
        }
        u64 elapsed = stm_since(start);
        double seconds = stm_sec(elapsed);

        printf("SIMD elapsed time: %llu ticks, %f seconds\n", elapsed, seconds);
        printf(" - result: %llu spaces in %llu characters\n", result, str.length - 1);
    }

    str.deinit();
}