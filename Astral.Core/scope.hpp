#pragma once
#include "Linxc.h"

template<typename T>
struct ScopeOnly
{
    T *instance;
    ScopeOnly(T *instance)
    {
        this->instance = instance;
    }
    ~ScopeOnly()
    {
        instance->deinit();
    }
};
#define CONCATIMPL(a, b) a ## b
#define CONCAT(a, b) CONCATIMPL(a, b)
#define Scope(type, instance) ScopeOnly<type> CONCAT(temp, __LINE__) = ScopeOnly<type>(&instance);