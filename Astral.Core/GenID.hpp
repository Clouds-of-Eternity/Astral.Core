#pragma once
#include "IndexedList.hpp"

//Generational ID implementation

template <typename CacheDataType, typename IDType>
struct GenIDSource
{
    collections::IndexedList<CacheDataType> cacheData;
    
    inline GenIDSource()
    {
        cacheData = collections::IndexedList<CacheDataType>();
    }
    inline GenIDSource(IAllocator allocator)
    {
        cacheData = collections::IndexedList<CacheDataType>(allocator);
    }
    inline void deinit()
    {
        cacheData.deinit();
    }

    inline bool CheckValid(IDType IDInstance) const
    {
        return IDInstance.generation != 0 && IDInstance.ID < cacheData.list.count && cacheData.list.ptr[IDInstance.ID].currentGeneration == IDInstance.generation;
    }
    inline void DeleteID(IDType IDInstance)
    {
        if (CheckValid(IDInstance))
        {
            cacheData[IDInstance.ID].currentGeneration++;
            cacheData.Remove(IDInstance.ID);
        }
    }
    inline IDType NewID()
    {
        bool justCreated;
        u32 index = cacheData.AddDefault(justCreated);
        u32 generation;

        if (justCreated)
        {
            generation = 1;
            cacheData[index].currentGeneration = 1;
        }
        else
        {
            generation = cacheData[index].currentGeneration;
        }

        const IDType result = {index, generation};
        return result;
    }
};