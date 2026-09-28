#pragma once
#include "Linxc.h"

template<typename T>
struct ScopeOnly
{
    T *instance;
    bool suppressed;
    ScopeOnly(T *instance)
    {
        this->instance = instance;
        suppressed = false;
    }
    ~ScopeOnly()
    {
        if (!suppressed)
        {
            instance->deinit();
        }
    }
};
#define CONCATIMPL(a, b) a ## b
#define CONCAT(a, b) CONCATIMPL(a, b)
#define Scope(type, instance) ScopeOnly<type> CONCAT(temp, __LINE__) = ScopeOnly<type>(&instance)
#define ScopeVar(type, instance, name) ScopeOnly<type> name = ScopeOnly<type>(&instance)