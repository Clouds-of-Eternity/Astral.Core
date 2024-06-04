# Astral.Core
Astral.Core is an alternative to the C++ standard library, with a focus on arena based memory management in place of RAII, inspired by the Zig standard library. It is currently in active development, with new components being added as and when I require them in my other projects, such as [Astral.Canvas](https://github.com/Linx145/Astral.Canvas), of which this library was initially developed for.

WARNING: The coding style in the library is identical to that of my other works. Functions and types are PascalCase, variables are camelCase. C style casts are used, so are malloc, free, printf and fopen in place of their C++ counterparts. No inheritance is used, relying on passing function pointers instead.

## Usage
Drop the Astral.Core library into a folder within your project and you're good to go. However, if you are using Astral.Core/Json.hpp, you must create a blank C++ file and paste the following code in:
```cpp
#define ASTRALCORE_JSON_IMPL

#include "Json.hpp"
```
Astral.Core does not utilise the C++ standard library, and works on Windows and Posix systems.

## Functionality
* Vectors
* Unordered hashmaps and hashsets
* Heap arrays
* Arithmetic types: Matrices, vectors, etc (Currently only supports SSE SIMD, which is not enabled by default)
* Allocators (Arena Allocator and CAllocator)
* UTF8 text utilities
* Strings & StringBuilders
* UUIDs
* Multithreading functions (Condition variables, mutices, thread creation)
* Dynamic library loading
* Linked lists
* Json reading via Json::ParseJsonDocument, and writing via Json::JsonWriter
* Lists (Identical to vectors except they 'zero' initialize using the default constructor)
* IO functions (Read file, check file existence, create directories, iterate files in directories)
* Path functions (Get path extension, swap extension, get directory, get file name)
* FIFO queues
* Sorting (TimSort and BitonicSort)