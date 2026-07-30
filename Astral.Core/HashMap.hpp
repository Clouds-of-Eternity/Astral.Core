#pragma once

#define HASHMAP_MAX_WEIGHT 0.8f

#include "Linxc.h"
#include "List.hpp"
#include <stdio.h>

#ifndef foreach
#define foreach(instance, iterator) for (auto instance = iterator.Next(); !iterator.completed; instance = iterator.Next())
#endif

namespace collections
{
    template <typename K, typename V>
    struct HashMap
    {
        IAllocator allocator;
        struct Entry
        {
            K key;
            V value;

            inline Entry()
            {
                key = {};
                value = {};
            }
            inline Entry(K key, V value)
            {
                this->key = key;
                this->value = value;
            }
        };
        struct Bucket
        {
            bool initialized;
            collections::List<Entry> entries;

            Bucket()
            {
                initialized = false;
                entries = collections::List<Entry>();
            }
            Bucket(IAllocator allocator)
            {
                initialized = false;
                entries = collections::List<Entry>(allocator);
            }
        };
        def_delegate(HashFunc, u32, K);
        def_delegate(EqlFunc, bool, K, K);

        HashFunc hashFunc;
        EqlFunc eqlFunc;

        Bucket *buckets;
        usize bucketsCount;
        usize filledBuckets;
        usize count;

        inline HashMap()
        {
            this->allocator = IAllocator{};
            this->hashFunc = NULL;
            this->eqlFunc = NULL;
            this->count = 0;
            this->filledBuckets = 0;
            this->bucketsCount = 32;
            this->buckets = NULL;
        }
        inline HashMap(IAllocator myAllocator, HashFunc hashFunction, EqlFunc eqlFunc)
        {
            this->allocator = myAllocator;
            this->hashFunc = hashFunction;
            this->eqlFunc = eqlFunc;
            this->count = 0;
            this->filledBuckets = 0;
            this->bucketsCount = 32;
            this->buckets = (Bucket*)this->allocator.Allocate(this->bucketsCount * sizeof(Bucket));
            for (usize i = 0; i < this->bucketsCount; i++)
            {
                this->buckets[i] = Bucket(this->allocator);
            }
        }
        inline HashMap(IAllocator myAllocator, HashFunc hashFunction, EqlFunc eqlFunc, u32 bucketsCount)
        {
            this->allocator = myAllocator;
            this->hashFunc = hashFunction;
            this->eqlFunc = eqlFunc;
            this->count = 0;
            this->filledBuckets = 0;
            this->bucketsCount = bucketsCount;
            this->buckets = (Bucket*)this->allocator.Allocate(this->bucketsCount * sizeof(Bucket));
            for (usize i = 0; i < this->bucketsCount; i++)
            {
                this->buckets[i] = Bucket(this->allocator);
            }
        }
        inline void deinit()
        {
            if (buckets != NULL)
            {
                for (usize i = 0; i < bucketsCount; i++)
                {
                    if (buckets[i].initialized)
                    {
                        buckets[i].entries.deinit();
                    }
                    //buckets[i].entries.~();
                }
                allocator.FREEPTR(buckets);
            }
        }
        inline void Clear()
        {
            if (buckets != NULL)
            {
                for (usize i = 0; i < bucketsCount; i++)
                {
                    if (buckets[i].initialized)
                    {
                        buckets[i].entries.Clear();
                    }
                }
                count = 0;
            }
        }
        inline void EnsureCapacity()
        {
            //in all likelihood, we may have to fill an additional bucket
            //on adding a new item. Thus, we may have to resize the underlying buffer if the weight
            //is more than 0.75
            if (filledBuckets + 1.0f >= bucketsCount * HASHMAP_MAX_WEIGHT)
            {
                usize newSize = bucketsCount * 2;

                Bucket *newBuckets = (Bucket*)this->allocator.Allocate(newSize * sizeof(Bucket));

                for (usize i = 0; i < newSize; i++)
                {
                    newBuckets[i] = Bucket(this->allocator);
                }
                for (usize i = 0; i < bucketsCount; i++)
                {
                    if (buckets[i].initialized)
                    {
                        for (usize j = 0; j < buckets[i].entries.count; j++)
                        {
                            u32 newKeyHash = hashFunc(buckets[i].entries.ptr[j].key);
                            usize newIndex = newKeyHash % newSize;
                            newBuckets[newIndex].initialized = true;
                            newBuckets[newIndex].entries.Add(Entry(buckets[i].entries.ptr[j].key, buckets[i].entries.ptr[j].value));
                        }
                        buckets[i].entries.deinit();
                    }
                }

                this->allocator.FREEPTR(buckets);
                buckets = newBuckets;
                bucketsCount = newSize;
            }
        }

        inline V* Add(K key, V value)
        {
            EnsureCapacity();
            u32 hash = hashFunc(key);
            usize index = hash % bucketsCount;

            if (!buckets[index].initialized)
            {
                buckets[index].initialized = true;

                filledBuckets++;
            }
            for (usize i = 0; i < buckets[index].entries.count; i++)
            {
                if (eqlFunc(buckets[index].entries.Get(i)->key, key))
                {
                    buckets[index].entries.Get(i)->value = value;
                    return &buckets[index].entries.Get(i)->value;
                }
            }
            count++;

            Entry newEntry = Entry(key, value);

            buckets[index].entries.Add(newEntry);
            return &buckets[index].entries.Get(buckets[index].entries.count - 1)->value;
        }

        inline bool Remove(K key)
        {
            u32 hash = hashFunc(key);
            usize index = hash % bucketsCount;

            if (buckets[index].initialized)
            {
                for (usize i = 0; i < buckets[index].entries.count; i++)
                {
                    if (eqlFunc(buckets[index].entries.Get(i)->key, key))
                    {
                        buckets[index].entries.RemoveAt_Swap(i);

                        count--;

                        return true;
                    }
                }
            }
            return false;
        }
        inline bool RemoveAndDeinitKey(K key)
        {
            u32 hash = hashFunc(key);
            usize index = hash % bucketsCount;

            if (buckets[index].initialized)
            {
                for (usize i = 0; i < buckets[index].entries.count; i++)
                {
                    if (eqlFunc(buckets[index].entries.Get(i)->key, key))
                    {
                        buckets[index].entries[i].key.deinit();
                        buckets[index].entries.RemoveAt_Swap(i);

                        count--;

                        return true;
                    }
                }
            }
            return false;
        }
        inline V Pop(K key)
        {
            u32 hash = hashFunc(key);
            usize index = hash % bucketsCount;

            if (buckets[index].initialized)
            {
                for (usize i = 0; i < buckets[index].entries.count; i++)
                {
                    if (eqlFunc(buckets[index].entries.Get(i)->key, key))
                    {
                        V result = buckets[index].entries[i].value;
                        buckets[index].entries.RemoveAt_Swap(i);

                        count--;

                        return result;
                    }
                }
            }
            return V();
        }

        inline V *Get(K key) const
        {
            if (buckets == NULL || eqlFunc == NULL || hashFunc == NULL)
            {
                return NULL;
            }
            u32 hash = hashFunc(key);
            usize index = hash % bucketsCount;

            if (buckets[index].initialized)
            {
                for (usize i = 0; i < buckets[index].entries.count; i++)
                {
                    if (eqlFunc(buckets[index].entries.Get(i)->key, key))
                    {
                        return &buckets[index].entries.Get(i)->value;
                    }
                }
            }
            return NULL;
        }

        inline V GetCopyOr(K key, V valueOnNotFound) const
        {
            if (buckets == NULL || eqlFunc == NULL || hashFunc == NULL)
            {
                return valueOnNotFound;
            }
            u32 hash = hashFunc(key);
            usize index = hash % bucketsCount;

            if (buckets[index].initialized)
            {
                for (usize i = 0; i < buckets[index].entries.count; i++)
                {
                    if (eqlFunc(buckets[index].entries.Get(i)->key, key))
                    {
                        return buckets[index].entries.Get(i)->value;
                    }
                }
            }
            return valueOnNotFound;
        }

        inline bool Contains(K key) const
        {
            u32 hash = hashFunc(key);
            usize index = hash % bucketsCount;

            if (!buckets[index].initialized)
            {
                return false;
            }

            for (usize i = 0; i < buckets[index].entries.count; i++)
            {
                if (eqlFunc(key, buckets[index].entries.Get(i)->key))
                {
                    return true;
                }
            }
            return false;
        }

#pragma region collections::HashMap<string, V> specific functions
#ifdef INCL_STRING
        inline V *GetWithCharSlice(CharSlice key) const
        {
            if (buckets == NULL)
            {
                return NULL;
            }
            u32 hash = CharSliceHash(key);
            usize index = hash % bucketsCount;

            if (buckets[index].initialized)
            {
                for (usize i = 0; i < buckets[index].entries.count; i++)
                {
                    if (buckets[index].entries.Get(i)->key == key)
                    {
                        return &buckets[index].entries.Get(i)->value;
                    }
                }
            }
            return NULL;
        }
        inline V GetCopyWithCharSliceOr(CharSlice key, V valueOnNotFound) const
        {
            if (buckets == NULL)
            {
                return valueOnNotFound;
            }
            u32 hash = CharSliceHash(key);
            usize index = hash % bucketsCount;

            if (buckets[index].initialized)
            {
                for (usize i = 0; i < buckets[index].entries.count; i++)
                {
                    if (buckets[index].entries.Get(i)->key == key)
                    {
                        return buckets[index].entries.Get(i)->value;
                    }
                }
            }
            return valueOnNotFound;
        }
        inline bool ContainsCharSlice(CharSlice key) const
        {
            u32 hash = CharSliceHash(key);
            usize index = hash % bucketsCount;

            if (!buckets[index].initialized)
            {
                return false;
            }

            for (usize i = 0; i < buckets[index].entries.count; i++)
            {
                if (buckets[index].entries.Get(i)->key == key)
                {
                    return true;
                }
            }
            return false;
        }
        inline bool RemoveWithCharSlice(CharSlice key)
        {
            u32 hash = CharSliceHash(key);
            usize index = hash % bucketsCount;

            if (buckets[index].initialized)
            {
                for (usize i = 0; i < buckets[index].entries.count; i++)
                {
                    if (buckets[index].entries.Get(i)->key == key)
                    {
                        buckets[index].entries.RemoveAt_Swap(i);

                        count--;

                        return true;
                    }
                }
            }
            return false;
        }
        inline bool RemoveAndDeinitKeyWithCharSlice(CharSlice key)
        {
            u32 hash = CharSliceHash(key);
            usize index = hash % bucketsCount;

            if (buckets[index].initialized)
            {
                for (usize i = 0; i < buckets[index].entries.count; i++)
                {
                    if (buckets[index].entries.Get(i)->key == key)
                    {
                        buckets[index].entries[i].key.deinit();
                        buckets[index].entries.RemoveAt_Swap(i);

                        count--;

                        return true;
                    }
                }
            }
            return false;
        }
        inline V PopWithCharSlice(CharSlice key)
        {
            u32 hash = CharSliceHash(key);
            usize index = hash % bucketsCount;

            if (buckets[index].initialized)
            {
                for (usize i = 0; i < buckets[index].entries.count; i++)
                {
                    if (buckets[index].entries.Get(i)->key == key)
                    {
                        V result = buckets[index].entries[i].value;
                        buckets[index].entries.RemoveAt_Swap(i);

                        count--;

                        return result;
                    }
                }
            }
            return V();
        }
#endif
#pragma endregion

        struct Iterator
        {
            const HashMap<K, V> *map;
            usize i;
            usize j;
            bool completed;

            Iterator(const HashMap<K, V> *map)
            {
                this->map = map;
                i = 0;
                j = 0;
                completed = false;
            }
            inline void Reset()
            {
                i = 0;
                j = 0;
                completed = false;
            }

            inline Entry* Next()
            {
                if (i >= map->bucketsCount || completed)
                {
                    return NULL;
                }
                while (!map->buckets[i].initialized || j >= map->buckets[i].entries.count)
                {
                    i++;
                    if (i >= map->bucketsCount)
                    {
                        completed = true;
                        return NULL;
                    }
                    if (map->buckets[i].initialized && map->buckets[i].entries.count > 0)
                    {
                        j = 0;
                        break;
                    }
                }
                return &map->buckets[i].entries.ptr[j++];
            }
        };
        inline Iterator GetIterator() const
        {
            return Iterator(this);
        }
    };
}