#pragma once
#include "Allocators.hpp"

namespace collections
{
	template<typename T>
	struct LinkedList;
	template<typename T>
	struct LinkedNode;

	template<typename T>
	struct LinkedNode
	{
		LinkedNode<T>* next;
		LinkedNode<T>* prev;
		LinkedList* list;
		T value;
	};

	template<typename T>
	struct LinkedList
	{
		IAllocator allocator;
		LinkedNode<T>* first;
		LinkedNode<T>* last;
		usize count;

		LinkedList()
		{
			allocator = IAllocator{};
			first = NULL;
			last = NULL;
			count = 0;
		}
		LinkedList(IAllocator myAllocator)
		{
			allocator = myAllocator;
			first = NULL;
			last = NULL;
			count = 0;
		}
		bool AddListAfter(LinkedNode<T>* after, LinkedList<T>* listToAdd)
		{
			//operation can only continue if listToAdd has objects within, and the allocator is the same instance
			if (listToAdd->allocator != this->allocator || listToAdd->first == NULL || listToAdd->last == NULL)
			{
				return false;
			}
			//transfer list ownership
			LinkedNode<T>* node = listToAdd->first;
			while (node != NULL)
			{
				node->list = this;
				node = node->next;
			}

			LinkedNode<T>* originalNext = after->next;
			after->next = listToAdd->first;
			listToAdd->first->prev = after;
			originalNext->prev = listToAdd->last;
			listToAdd->last->next = originalNext;
			this->count += listToAdd->count;

			listToAdd->count = 0;
			listToAdd->first = NULL;
			listToAdd->last = NULL;
			return true;
		}
		LinkedNode<T>* AddBefore(T item, LinkedNode<T>* before)
		{
			LinkedNode<T>* node = (LinkedNode<T>*)allocator.Allocate(sizeof(LinkedNode<T>));
			node->list = this;
			node->value = item;
			LinkedNode<T>* originalPrev = before->prev;
			before->prev = node;
			node->next = before;
			node->prev = originalPrev;
			if (originalPrev != NULL)
			{
				originalPrev->next = node;
			}
			else
			{
				node->list->first = node;
			}
			count += 1;
			return node;
		}
		LinkedNode<T>* AddAfter(T item, LinkedNode<T>* after)
		{
			LinkedNode<T>* node = (LinkedNode<T>*)allocator.Allocate(sizeof(LinkedNode<T>));
			node->list = this;
			node->value = item;
			LinkedNode<T>* originalNext = after->next;
			after->next = node;
			node->prev = after;
			node->next = originalNext;
			if (originalNext != NULL)
			{
				originalNext->prev = node;
			}
			else
			{
				node->list->last = node;
			}
			count += 1;
			return node;
		}
		LinkedNode<T>* Append(T item)
		{
			if (this->last != NULL)
			{
				return AddAfter(item, this->last);
			}
			LinkedNode<T>* node = (LinkedNode<T>*)allocator.Allocate(sizeof(LinkedNode<T>));
			node->list = this;
			node->value = item;
			node->prev = NULL;
			node->next = NULL;
			this->first = node;
			this->last = node;
			count += 1;
			return node;
		}
		LinkedNode<T>* Prepend(T item)
		{
			if (this->first != NULL)
			{
				return AddBefore(item, this->first);
			}
			LinkedNode<T>* node = (LinkedNode<T>*)allocator.Allocate(sizeof(LinkedNode<T>));
			node->list = this;
			node->value = item;
			node->prev = NULL;
			node->next = NULL;
			this->first = node;
			this->last = node;
			count += 1;
			return node;
		}
		void Remove(LinkedNode<T>* node, bool dispose = true)
		{
			if (node->next == NULL)
			{
				node->list->last = node->prev;
			}
			else
			{
				node->next->prev = node->prev;
			}

			if (node->prev == NULL)
			{
				node->list->first = node->next;
			}
			else
			{
				node->prev->next = node->next;
			}
			node->list->count -= 1;
			if (dispose)
			{
				node->list->allocator.Free(node);
			}
		}
		void Clear_Free()
		{
			LinkedNode<T>* node = this->first;
			while (node != NULL)
			{
				LinkedNode<T>* next = node->next;
				this->allocator.Free(node);
				node = next;
			}
			count = 0;
		}
	};
}