#pragma once
#include "Linxc.h"

def_delegate(ACTestFunc, bool);

#define ACASSERT(expression) if(!(expression)) { printf("Assertion failed! %s @ file %s, line %u\n", #expression, __FILE__, __LINE__); return false; }