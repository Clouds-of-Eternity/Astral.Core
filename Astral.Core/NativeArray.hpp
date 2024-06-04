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