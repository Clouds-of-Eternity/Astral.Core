#pragma once
#include "List.h"

#ifndef HASHMAP_MAX_WEIGHT
#define HASHMAP_MAX_WEIGHT 0.8f
#endif

#ifndef foreach
#define foreach(instance, iterator) for (void *instance = HashMapIterator_Next(&iterator); !iterator.completed; instance = HashMapIterator_Next(&iterator))
#endif

def_delegate(MapHashFunc, uint32_t, const void *);
def_delegate(MapEqlFunc, bool, const void *, const void *);

typedef struct HashMapBucket
{
    bool initialized;
    List entries;
} HashMapBucket;

static inline HashMapBucket HashMapBucket_Create(IAllocator allocator, size_t keySize, size_t valueSize)
{
    HashMapBucket result;
    result.initialized = false;
    result.entries = List_Create(allocator, keySize + valueSize);
    return result;
}

typedef struct HashMap
{
    IAllocator allocator;

    MapHashFunc hashFunc;
    MapEqlFunc eqlFunc;

    HashMapBucket *buckets;
    size_t bucketsCount;
    size_t filledBuckets;
    size_t count;

    size_t keySize;
    size_t valueSize;
} HashMap;

static inline HashMap HashMap_Empty()
{
    HashMap result = {};
    return result;
}
static inline HashMap HashMap_Create(size_t keySize, size_t valueSize, IAllocator allocator, MapHashFunc hashFunc, MapEqlFunc eqlFunc)
{
    HashMap result = {allocator, hashFunc, eqlFunc, NULL, 32, 0, 0, keySize, valueSize};
    result.buckets = IAllocator_Allocate(allocator, sizeof(HashMapBucket) * result.bucketsCount);
    for (size_t i = 0; i < result.bucketsCount; i++)
    {
        result.buckets[i] = HashMapBucket_Create(allocator, keySize, valueSize);
    }
    return result;
}
#define HASHMAP_CREATE(keyType, valueType, allocator, hashFunc, eqlFunc) HashMap_Create(sizeof(keyType), sizeof(valueType), allocator, hashFunc, eqlFunc)

static inline void HashMap_Deinit(HashMap *self)
{
    if (self->buckets != NULL)
    {
        for (size_t i = 0; i < self->bucketsCount; i++)
        {
            if (self->buckets[i].initialized)
            {
                List_Deinit(&self->buckets[i].entries);
            }
        }
        IAllocator_Free(self->allocator, self->buckets);
    }
}
static inline void HashMap_Clear(HashMap *self)
{
    if (self->buckets != NULL)
    {
        for (size_t i = 0; i < self->bucketsCount; i++)
        {
            if (self->buckets[i].initialized)
            {
                List_Clear(&self->buckets[i].entries);
                //buckets[i].entries.Clear();
            }
        }
        self->count = 0;
    }
}
static inline void HashMap_EnsureCapacity(HashMap *self)
{
    if (self->filledBuckets + 1.0f >= self->bucketsCount * HASHMAP_MAX_WEIGHT)
    {
        size_t newSize = self->bucketsCount * 2;

        HashMapBucket *newBuckets = (HashMapBucket*)IAllocator_Allocate(self->allocator, newSize * sizeof(HashMapBucket));

        for (size_t i = 0; i < newSize; i++)
        {
            newBuckets[i] = HashMapBucket_Create(self->allocator, self->keySize, self->valueSize);//Bucket(this->allocator);
        }
        for (size_t i = 0; i < self->bucketsCount; i++)
        {
            if (self->buckets[i].initialized)
            {
                for (size_t j = 0; j < self->buckets[i].entries.count; j++)
                {
                    const void *key = List_Get(&self->buckets[i].entries, j);
                    
                    uint32_t newKeyHash = self->hashFunc(key);
                    size_t newIndex = newKeyHash % newSize;
                    newBuckets[newIndex].initialized = true;
                    //should copy over both key and value
                    List_Add(&newBuckets[newIndex].entries, key);
                }
                List_Deinit(&self->buckets[i].entries);
            }
        }

        IAllocator_Free(self->allocator, self->buckets);
        self->buckets = newBuckets;
        self->bucketsCount = newSize;
    }
}

static inline void *HashMap_Add(HashMap *self, const void *key, const void *value)
{
    HashMap_EnsureCapacity(self);

    uint32_t hash = self->hashFunc(key);
    size_t index = hash % self->bucketsCount;

    if (!self->buckets[index].initialized)
    {
        self->buckets[index].initialized = true;

        self->filledBuckets++;
    }
    for (size_t i = 0; i < self->buckets[index].entries.count; i++)
    {
        void *keyAt = List_Get(&self->buckets[index].entries, i);
        if (self->eqlFunc(keyAt, key))
        {
            memcpy(keyAt, key, self->keySize);

            void *result = (uint8_t *)keyAt + self->keySize;
            memcpy(result, value, self->valueSize);
            //buckets[index].entries.Get(i)->value = value;
            return result;
        }
    }
    self->count++;

    const size_t entryIndex = self->buckets[index].entries.count;
    List_AddEmpty(&self->buckets[index].entries);

    void *keyOffset = (uint8_t *)self->buckets[index].entries.ptr + entryIndex * (self->keySize + self->valueSize);
    void *valueOffset = (uint8_t *)keyOffset + self->keySize;
    memcpy(keyOffset, key, self->keySize);
    memcpy(valueOffset, value, self->valueSize);

    return valueOffset;
}

static inline bool HashMap_Remove(HashMap *self, const void *key)
{
    uint32_t hash = self->hashFunc(key);
    size_t index = hash % self->bucketsCount;

    if (self->buckets[index].initialized)
    {
        for (size_t i = 0; i < self->buckets[index].entries.count; i++)
        {
            void *keyAt = List_Get(&self->buckets[index].entries, i);
            if (self->eqlFunc(keyAt, key))
            {
                //buckets[index].entries.RemoveAt_Swap(i);
                List_RemoveAtSwap(&self->buckets[index].entries, i);

                self->count--;

                return true;
            }
        }
    }
    return false;
}
static inline void *HashMap_Get(const HashMap *self, const void *key)
{
    uint32_t hash = self->hashFunc(key);
    size_t index = hash % self->bucketsCount;

    if (self->buckets[index].initialized)
    {
        for (size_t i = 0; i < self->buckets[index].entries.count; i++)
        {
            void *keyAt = List_Get(&self->buckets[index].entries, i);
            if (self->eqlFunc(keyAt, key))
            {
                void *valueAt = (uint8_t *)keyAt + self->keySize;
                return valueAt;
            }
        }
    }
    return NULL;
}
static inline bool HashMap_Contains(const HashMap *self, const void *key)
{
    return HashMap_Get(self, key) != NULL;
}

#define HM_GET(type, hashmapPtr, keyPtr) *(type *)HashMap_Get(hashmapPtr, keyPtr)

typedef struct HashMapIterator
{
    HashMap *map;
    size_t i;
    size_t j;
    bool completed;
} HashMapIterator;

static inline HashMapIterator HashMapIterator_From(HashMap *self)
{
    const HashMapIterator result = {self, 0, 0, false};
    return result;
}

static inline void *HashMapIterator_Next(HashMapIterator *self)
{
    if (self->i >= self->map->bucketsCount || self->completed)
    {
        return NULL;
    }
    while (!self->map->buckets[self->i].initialized || self->j >= self->map->buckets[self->i].entries.count)
    {
        self->i++;
        if (self->i >= self->map->bucketsCount)
        {
            self->completed = true;
            return NULL;
        }
        if (self->map->buckets[self->i].initialized && self->map->buckets[self->i].entries.count > 0)
        {
            self->j = 0;
            break;
        }
    }
    void *entry = List_Get(&self->map->buckets[self->i].entries, self->j);
    self->j += 1;
    return entry;
}

static inline void *HashMapEntry_GetValue(void *entry, HashMap *hashMap)
{
    return (uint8_t *)entry + hashMap->keySize;
}

#define HMENTRY_GETKEY(entry, keyType, hm) (keyType *)entry
#define HMENTRY_GETVAL(entry, valType, hm) (valType *)HashMapEntry_GetValue(entry, hm )