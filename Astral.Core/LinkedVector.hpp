#pragma once
#include "vector.hpp"

namespace collections
{
	template<typename T>
	struct LinkedVecNode
	{
		i32 next;
		i32 prev;
		T value;

		inline LinkedVecNode()
		{
			next = -1;
			prev = -1;
		}
	};

	template<typename T>
	struct LinkedVector
	{
		IAllocator allocator;
		collections::vector<LinkedVecNode<T>> elements;
		collections::vector<i32> freeIndices;
		i32 firstIndex;
		i32 lastIndex;
		usize count;

		LinkedVector()
		{
			allocator = IAllocator{};
			elements = collections::vector<LinkedVecNode<T>>();
			freeIndices = collections::vector<i32>();
			firstIndex = -1;
			lastIndex = -1;
			count = 0;
		}
		LinkedVector(IAllocator myAllocator)
		{
			allocator = myAllocator;
			elements = collections::vector<LinkedVecNode<T>>(allocator);
			freeIndices = collections::vector<i32>(allocator);
			firstIndex = -1;
			lastIndex = -1;
			count = 0;
		}
		inline LinkedVecNode<T> GetNode(u32 index)
		{
			return elements.ptr[index];
		}
		inline T *GetNodeValue(u32 index)
		{
			return &elements.ptr[index].value;
		}
		inline i32 GetNewIndex()
		{
			i32 newIndex;
			if (freeIndices.count > 0)
			{
				newIndex = freeIndices.Pop();
			}
			else
			{
				newIndex = elements.count;
				elements.Add(LinkedVecNode<T>());
			}
			return newIndex;
		}
		i32 AddBefore(T item, i32 before)
		{
			i32 newIndex = GetNewIndex();

			LinkedVecNode<T> *beforeNode = &elements.ptr[before];
			LinkedVecNode<T> *newNode = &elements.ptr[newIndex];
			
			newNode->value = item;

			if (beforeNode->prev != -1)
			{
				LinkedVecNode<T> *originalPrevNode = &elements.ptr[beforeNode->prev];
				originalPrevNode->next = newIndex;
				newNode->prev = beforeNode->prev;
			}
			
			newNode->next = before;
			beforeNode->prev = newIndex;
			if (firstIndex == before)
			{
				firstIndex = newIndex;
			}
			return newIndex;
		}
		i32 AddAfter(T item, i32 after)
		{
			i32 newIndex = GetNewIndex();

			LinkedVecNode<T> *afterNode = &elements.ptr[after];
			LinkedVecNode<T> *newNode = &elements.ptr[newIndex];
			
			newNode->value = item;

			if (afterNode->next != -1)
			{
				LinkedVecNode<T> *originalNextNode = &elements.ptr[afterNode->next];
				originalNextNode->prev = newIndex;
				newNode->next = afterNode->next;
			}
			
			newNode->prev = after;
			afterNode->next = newIndex;
			if (lastIndex == newIndex)
			{
				lastIndex = newIndex;
			}
			return newIndex;
		}
		i32 Append(T item)
		{
			if (lastIndex != -1)
			{
				return AddAfter(item, lastIndex);
			}

			i32 newIndex = GetNewIndex();
			LinkedVecNode<T> *node = &elements.ptr[newIndex];

			node->value = item;
			node->next = -1;
			node->prev = -1;

			firstIndex = newIndex;
			lastIndex = newIndex;
			return newIndex;
		}
		i32 Prepend(T item)
		{
			if (firstIndex != -1)
			{
				return AddBefore(item, firstIndex);
			}

			i32 newIndex = GetNewIndex();
			LinkedVecNode<T> *node = &elements.ptr[newIndex];

			node->value = item;
			node->next = -1;
			node->prev = -1;

			firstIndex = newIndex;
			lastIndex = newIndex;
			return newIndex;
		}
		void Remove(i32 nodeIndex)
		{
			LinkedVecNode<T> *node = &elements.ptr[nodeIndex];
			if (node->prev != -1)
			{
				LinkedVecNode<T> *prevNode = &elements.ptr[node->prev];
				prevNode->next = node->next;
			}
			else
			{
				firstIndex = node->next;
			}
			if (node->next != -1)
			{
				LinkedVecNode<T> *nextNode = &elements.ptr[node->next];
				nextNode->prev = node->prev;
			}
			else
			{
				lastIndex = node->prev;
			}
			node->prev = -1;
			node->next = -1;
			freeIndices.Add(nodeIndex);
		}
		void Clear()
		{
			freeIndices.Clear();
			elements.Clear();
			firstIndex = -1;
			lastIndex = -1;
		}

		inline void deinit()
		{
			freeIndices.deinit();
			elements.deinit();
		}
	};
}