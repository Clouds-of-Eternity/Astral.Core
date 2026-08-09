#pragma once
#include "string.hpp"
#include "List.hpp"
#include "SegmentedList.hpp"
#include "HashMap.hpp"
#include "HashSet.hpp"
#include "uuid.hpp"

#include "CPPTests/ACAssert.h"

inline bool TestHashMap()
{
    printf(" * Running TestHashMap...\n");
    const u32 itemsCount = 100;
    Random randInstance = Random();
    randInstance.Seed(413);

    uuid keys[itemsCount];
    string values[itemsCount];

    collections::HashMap<uuid, string> IDtoStr = collections::HashMap<uuid, string>(GetCAllocator(), &UuidHash, &UuidEql);
    collections::HashMap<string, uuid> StrtoID = collections::HashMap<string, uuid>(GetCAllocator(), &stringHash, &stringEql);

    for (u32 i = 0; i < itemsCount; i++)
    {
        keys[i] = uuid::New(&randInstance);
        values[i] = keys[i].ToString(GetCAllocator());

        IDtoStr.Add(keys[i], values[i]);
        StrtoID.Add(values[i], keys[i]);
    }

    for (u32 i = 0; i < itemsCount; i++)
    {
        CharSlice valueAsCharSlice = values[i];
        ACASSERT(IDtoStr.GetCopyOr(keys[i], string()) == values[i]);
        ACASSERT(StrtoID.GetCopyOr(values[i], uuid()) == keys[i]);
        ACASSERT(StrtoID.GetCopyWithCharSliceOr(valueAsCharSlice, uuid()) == keys[i]);

        IDtoStr.Remove(keys[i]);
        StrtoID.RemoveAndDeinitKey(values[i]);
    }

    return true;
}
inline bool TestSegmentedList()
{
    printf(" * Running TestSegmentedList...\n");
    collections::SegmentedList<u32> list = collections::SegmentedList<u32>(GetCAllocator(), 16);

    //u32 *latestPtr = 0;
    u32 *latestPtrs[10];
    const u32 intendedValues[10] = {
        0,
        9,
        18,
        27,
        36,
        45,
        54,
        63,
        72,
        81
    };
    i32 j = -1;
    for (u32 i = 0; i < 100; i++)
    {
        if (i % 10 == 0)
        {
            j++;
            latestPtrs[j] = list.Add(0);
        }
        else
        {
            *latestPtrs[j] += j;
            list.Add(i);
        }
    }
    
    usize removed = 0;
    //printf("Results:\n");
    for (u32 i = 0; i < 10; i++)
    {
        if (i % 2 == 0)
        {
            //remove item at position 1 a total of 5 times. If the swap happens
            //correctly, the values should remain unchanged.
            list.RemoveAt_Swap(1);
        }
        u32 finalValue = *(latestPtrs[i]);
        //printf(" - value %i: %u\n", i, finalValue);
        ACASSERT(finalValue == intendedValues[i]);
    }

    return true;
}

inline void RunCollectionsTests()
{
    printf("Executing RunCollectionsTests()...\n----\n");
    ACTestFunc tests[] = {
        TestHashMap,
        TestSegmentedList
    };
    const usize total = sizeof(tests) / sizeof(ACTestFunc);
    u32 passes = total;

    for (u32 i = 0; i < total; i++)
    {
        if (!tests[i]())
        {
            passes--;
        }
    }
    printf("RunCollectionsTests() passed: %u/%u\n", passes, total);
}