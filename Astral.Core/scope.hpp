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
#define Scope(type, instance) ScopeOnly<type> var##__LINE__ = ScopeOnly<type>(&instance);