#pragma once
#include "Linxc.h"
#include "array.hpp"
#include "assert.h"
#include "Maths/Util.hpp"

#define SORT_SUBARRAY_SIZE 32

template<typename T>
void InsertionSort(T* array, usize left, usize right, i8(*comparator)(T&, T&))
{
    for (i64 i = left + 1; i <= right; i++) 
    { 
        T temp = array[i]; 
        i64 j = i - 1; 
        while (j >= left && comparator(array[j], temp) > 0) 
        { 
            array[j + 1] = array[j]; 
            j--;
            if (j == -1) //WHY IS THIS NEEDED?
            {
                break;
            }
        }
        array[j + 1] = temp; 
    }
}

template<typename T>
void MergeSort(T* array, i64 left, i64 mid, i64 right, i8(*comparator)(T&, T&))
{
    i64 leftArrLength = mid - left + 1;
    i64 rightArrLength = right - mid;
    //Original array is broken into two parts: left and right subarray
    collections::Array<T> leftArr = collections::Array<T>(GetCAllocator(), leftArrLength);
    collections::Array<T> rightArr = collections::Array<T>(GetCAllocator(), rightArrLength);

    //Fill in the subarrays
    for (i64 index = 0; index < leftArrLength; index++)
        leftArr.data[index] = array[left + index];
    for (i64 index = 0; index < rightArrLength; index++)
        rightArr.data[index] = array[mid + 1 + index];

    //Merge the subarrays after comparing
    int i = 0, j = 0, k = left;
    while (i < leftArrLength && j < rightArrLength)
    {
        if (comparator(leftArr.data[i], rightArr.data[j]) <= 0)//(leftArr[i]?.CompareTo(rightArr[j]) ?? 0) <= 0)
            array[k++] = leftArr.data[i++];
        else
            array[k++] = rightArr.data[j++];
    }

    //Copy any remaining elements left, if any
    while (i < leftArrLength)
        array[k++] = leftArr.data[i++];
    while (j < rightArrLength)
        array[k++] = rightArr.data[j++];

    leftArr.deinit();
    rightArr.deinit();
}

template<typename T>
void BitonicSort(T* array, usize arrayLength)
{
    //k: stage
    //j: pass
    //i: array index
    for (i32 k = 2; k <= arrayLength / 2; k *= 2)
    {
        for (i32 j = k / 2; j > 0; j /= 2)
        {
            for (i32 i = 0; i < arrayLength; i++) 
            {
                i32 ij = i ^ j;

                if (ij > i) 
                {
                    if ((i & k) == 0) 
                    {
                        if (array[i] > array[ij])
                        {
                            T temp = array[i];
                            array[i] = array[ij];
                            array[ij] = temp;
                        }
                    }
                    else 
                    {
                        if (array[i] < array[ij])
                        {
                            T temp = array[i];
                            array[i] = array[ij];
                            array[ij] = temp;
                        }
                    }
                }
            }
        }
    }
}

//32 is used as a compromize between fast Insertion Sorts and fewer merges
//A power of two is used for the most efficiency, since the resulting arrays will also be powers of two (unless the array length isn't, then the last one will just be the remainder.)
//Insertion Sort is very efficient with small arrays, but not with large arrays.  A value of 64 or higher results in slower initial sorting, but faster merging
//A value of 16 or lower results in faster initial sorting, but slower merging since more merging has to be done
#define SUBARRAY_SIZE 32

template<typename T> 
void TimSort(T* array, usize arrayLength, i8(*comparator)(T&, T&))
{
    if (arrayLength < 2)
        return;

    //Sort the subarrays using insertion sort
    for (i64 i = 0; i < arrayLength; i += SUBARRAY_SIZE)
        InsertionSort<T>(array, i, MIN(i + SUBARRAY_SIZE - 1, arrayLength - 1), comparator);

    if (arrayLength <= SUBARRAY_SIZE)
        return;

    //Merge the subarrays
    for (i64 size = SUBARRAY_SIZE; size < arrayLength; size *= 2)
    {
        for (i64 left = 0; left < arrayLength; left += 2 * size)
        {
            i64 mid = left + size - 1;
            i64 right = MIN(left + (2 * size) - 1, arrayLength - 1);

            //Sanity check for if the left subarray is the final subarray
            //'mid' ends up being greater than 'count' if this were the case, which causes problems in
            //  Merge() which sets the length of the right subarray to 'right - mid'
            if (mid >= arrayLength)
                continue;

            MergeSort<T>(array, left, mid, right, comparator);
        }
    }
}

#define IMPL_COMPARATORS(nameOfType, compareVar) inline bool operator<(nameOfType &other) { return this->compareVar < other.compareVar;} inline bool operator>(nameOfType &other) { return this->compareVar > other.compareVar;} inline bool operator<=(nameOfType &other) { return this->compareVar <= other.compareVar;} inline bool operator>=(nameOfType &other) { return this->compareVar >+ other.compareVar;} inline bool operator==(nameOfType &other) { return this->compareVar == other.compareVar;} inline bool operator!=(nameOfType &other) { return this->compareVar != other.compareVar;}

template<typename T>
i8 OperatorBasedComparator(T& A, T& B)
{
    if (A < B)
    {
        return -1;
    }
    if (A > B)
    {
        return 1;
    }
    return 0;
}