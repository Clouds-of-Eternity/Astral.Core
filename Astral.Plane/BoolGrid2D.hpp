#pragma once
#include "Maths/Vec2.hpp"
#include "BitFlags.hpp"
#include "HashMap.hpp"
#include <string.h>
#include <assert.h>

#ifndef BOOL_GRID_CHUNK_SIZE
#define BOOL_GRID_CHUNK_SIZE 16
#endif

#define BOOL_GRID_STATES (BOOL_GRID_CHUNK_SIZE * BOOL_GRID_CHUNK_SIZE / 8)

static_assert(BOOL_GRID_CHUNK_SIZE <= 128, "BOOL_GRID_CHUNK_SIZE must be less than or equal to 128");

struct BoolGridChunk2D
{
    //total cells required = BOOL_GRID_CHUNK_SIZE * BOOL_GRID_CHUNK_SIZE
    //cells per u8: 8
    //total states required: (BOOL_GRID_CHUNK_SIZE * BOOL_GRID_CHUNK_SIZE) / 8
    u8 states[BOOL_GRID_STATES];
    Maths::Point2 position;
    /// @brief A value from 0 to the square of BOOL_GRID_CHUNK_SIZE determining how many
    /// cells are in the 'true' state.
    i16 occupancy;
    u16 flags;

    inline bool GetCustomFlagValue(u8 bitIndex) const
    {
        return CheckBitFlag(&flags, 1, bitIndex + 1);
    }
    inline void SetCustomFlagValue(bool value, u8 bitIndex)
    {
        SetBitFlag(&flags, 1, value, bitIndex + 1);
    }
    inline bool GetDefaultValue() const
    {
        return CheckBitFlag(&flags, 1, 0);
    }
    inline void SetDefaultValue(bool value)
    {
        SetBitFlag(&flags, 1, value, 0);
    }

    inline BoolGridChunk2D()
    {
        flags = 0;
        memset(states, (i32)flags, sizeof(u8) * BOOL_GRID_STATES);
        position = Maths::Point2();
        occupancy = 0;
    }
    inline BoolGridChunk2D(Maths::Point2 chunkPosition, bool defaultValue)
    {
        SetDefaultValue(defaultValue);
        memset(states, (i32)flags, sizeof(u8) * BOOL_GRID_STATES);
        position = chunkPosition;
        occupancy = defaultValue ? BOOL_GRID_CHUNK_SIZE * BOOL_GRID_CHUNK_SIZE : 0;
    }
    /// @brief Checks and returns the current boolean value at the cell position
    /// @param X The 8-bit unsigned X position of the cell within this chunk itself, relative to the chunk's origin.
    /// @param Y The 8-bit unsigned Y position of the cell within this chunk itself, relative to the chunk's origin.
    /// @return The current state of the cell.
    inline bool CheckThis(u8 X, u8 Y) const
    {
        const u32 bitIndex = X + Y * BOOL_GRID_CHUNK_SIZE;
        const u32 sizeTBits = sizeof(u8) * 8;
        const u32 ptrIndex = bitIndex / sizeTBits;
        const u32 bitOffset = bitIndex % sizeTBits;

        if (ptrIndex >= BOOL_GRID_STATES)
        {
            return false;
        }
        return (states[ptrIndex] & (1 << bitOffset)) != 0;
    }
    /// @brief Sets the current boolean value at the cell position
    /// @param X The 8-bit unsigned X position of the cell within this chunk itself, relative to the chunk's origin.
    /// @param Y The 8-bit unsigned Y position of the cell within this chunk itself, relative to the chunk's origin.
    /// @param state The state to set the cell to.
    /// @return The previous state of the cell.
    inline bool SetThis(u8 X, u8 Y, u8 state)
    {
        assert(X >= 0 && Y >= 0 && X < BOOL_GRID_CHUNK_SIZE && Y < BOOL_GRID_CHUNK_SIZE);

        const u32 bitIndex = X + Y * BOOL_GRID_CHUNK_SIZE;
        const u32 sizeTBits = sizeof(u8) * 8;
        const u32 ptrIndex = bitIndex / sizeTBits;
        const u32 bitOffset = bitIndex % sizeTBits;

        assert(ptrIndex < BOOL_GRID_STATES);

        const u8 flag = 1 << bitOffset;
        const bool prevState = (states[ptrIndex] & flag) != 0;
        //clear bit
        states[ptrIndex] = states[ptrIndex] & (~flag);
        //if value is true, toggle bit
        if (state)
        {
            states[ptrIndex] = states[ptrIndex] | flag;
        }
        if (state && !prevState)
        {
            occupancy++;
        }
        else if (!state && prevState)
        {
            occupancy--;
        }
        assert(occupancy >= 0 && occupancy <= BOOL_GRID_CHUNK_SIZE * BOOL_GRID_CHUNK_SIZE);

        return prevState;
    }
};

struct DynamicBoolGrid2D
{
    collections::HashMap<Maths::Point2, BoolGridChunk2D> chunks;
    bool defaultValue;

    inline DynamicBoolGrid2D()
    {
        chunks = collections::HashMap<Maths::Point2, BoolGridChunk2D>();
        defaultValue = false;
    }
    inline DynamicBoolGrid2D(IAllocator allocator, bool defaultCellValue)
    {
        chunks = collections::HashMap<Maths::Point2, BoolGridChunk2D>(allocator, &Maths::Point2Hash, &Maths::Point2Eql);
        defaultValue = defaultCellValue;
    }

    inline void Clear()
    {
        auto iterator = chunks.GetIterator();
        foreach (kvp, iterator)
        {
            memset(kvp->value.states, defaultValue, sizeof(u8) * BOOL_GRID_STATES);
        }
    }
    inline void deinit()
    {
        chunks.deinit();
    }
    inline bool Check(i64 X, i64 Y, BoolGridChunk2D **_Nullable cacheLastAccessedChunk) const
    {
        const Maths::Point2 chunkPos = Maths::Point2((i32)floorf(X / (float)BOOL_GRID_CHUNK_SIZE), (i32)floorf(Y / (float)BOOL_GRID_CHUNK_SIZE));

        BoolGridChunk2D *chunk = NULL;
        if (cacheLastAccessedChunk != NULL && *cacheLastAccessedChunk != NULL && (*cacheLastAccessedChunk)->position == chunkPos)
        {
            chunk = *cacheLastAccessedChunk;
        }
        else
        {
            chunk = chunks.Get(chunkPos);
        }
        if (cacheLastAccessedChunk != NULL)
        {
            *cacheLastAccessedChunk = chunk;
        }
        return chunk == NULL ? defaultValue : chunk->CheckThis((u8)(X - (i64)(chunkPos.X * BOOL_GRID_CHUNK_SIZE)), (u8)(Y - (i64)(chunkPos.Y * BOOL_GRID_CHUNK_SIZE)));
    }
    inline bool Set(i32 X, i32 Y, bool value, BoolGridChunk2D **_Nullable cacheLastAccessedChunk)
    {
        const Maths::Point2 chunkPos = Maths::Point2(floorf(X / (float)BOOL_GRID_CHUNK_SIZE), floorf(Y / (float)BOOL_GRID_CHUNK_SIZE));

        BoolGridChunk2D *chunk = NULL;
        if (cacheLastAccessedChunk != NULL && *cacheLastAccessedChunk != NULL && (*cacheLastAccessedChunk)->position == chunkPos)
        {
            chunk = *cacheLastAccessedChunk;
        }
        else
        {
            chunk = chunks.Get(chunkPos);
            if (chunk == NULL)
            {
                chunk = chunks.Add(chunkPos, BoolGridChunk2D(chunkPos, defaultValue));
            }
        }
        assert(chunk);
        bool prevState = chunk->SetThis((u8)(X - chunkPos.X * BOOL_GRID_CHUNK_SIZE), (u8)(Y - chunkPos.Y * BOOL_GRID_CHUNK_SIZE), value);
        if (cacheLastAccessedChunk != NULL)
        {
            *cacheLastAccessedChunk = chunk;
        }
        return prevState;
    }
    inline DynamicBoolGrid2D Clone(IAllocator allocator, u64 *_Nullable outTotalOccupancy) const
    {
        DynamicBoolGrid2D result = DynamicBoolGrid2D(allocator, this->defaultValue);
        auto iterator = this->chunks.GetIterator();
        u32 index = 0;
        foreach (kvp, iterator)
        {
            result.chunks.Add(kvp->key, kvp->value);
            if (outTotalOccupancy != NULL)
            {
                *outTotalOccupancy += (u64)kvp->value.occupancy;
            }
        }
        return result;
    }
    inline void CloneAllChunks(BoolGridChunk2D *output, u32 count, u64 *_Nullable outTotalOccupancy) const
    {
        if (count == 0)
        {
            return;
        }
        auto iterator = this->chunks.GetIterator();
        u32 index = 0;
        foreach (kvp, iterator)
        {
            output[index++] = kvp->value;
            if (outTotalOccupancy != NULL)
            {
                *outTotalOccupancy += (u64)kvp->value.occupancy;
            }
            if (index >= count)
            {
                break;
            }
        }
    }
};