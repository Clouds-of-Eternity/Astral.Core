#pragma once
#include "Linxc.h"
#include "assert.h"

template <typename T>
struct option
{
    T value;
    bool present;

    inline option()
    {
        present = false;
    }
    inline option(T value)
    {
        this->value = value;
        present = true;
    }

    inline T Unwrap()
    {
        assert(present);
        return value;
    }
    inline T ValueOr(T alternate)
    {
        return present ? value : alternate;
    }
};