#pragma once
#include "Linxc.h"

struct NativeArray
{
    void *data;
    usize length;

    inline NativeArray()
    {
        data = NULL;
        length = 0;
    }
    inline NativeArray(void* data, usize length)
    {
        this->data = data;
        this->length = length;
    }
};

struct ConstNativeArray
{
    const void *data;
    usize length;

    inline ConstNativeArray()
    {
        data = NULL;
        length = 0;
    }
    inline ConstNativeArray(NativeArray nativeArray)
    {
        data = nativeArray.data;
        length = nativeArray.length;
    }
    inline ConstNativeArray(const void *data, usize length)
    {
        this->data = data;
        this->length = length;
    }
};