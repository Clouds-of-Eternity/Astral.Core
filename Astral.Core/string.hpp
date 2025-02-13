#pragma once

#include "Linxc.h"
#include "stdlib.h"
#include "allocators.hpp"
#include "string.h"
#include "option.hpp"
#include "array.hpp"
#include "stdio.h"
#include "math.h"
#include "vector.hpp"

inline const char* digits2(usize value)
{
    return &"0001020304050607080910111213141516171819"
        "2021222324252627282930313233343536373839"
        "4041424344454647484950515253545556575859"
        "6061626364656667686970717273747576777879"
        "8081828384858687888990919293949596979899"[value * 2];
}

#ifdef WINDOWS
typedef wchar_t char_t;
#else
typedef char char_t;
#endif

struct string
{
    IAllocator allocator;
    char *buffer;
    usize length;

    inline string()
    {
        this->allocator = IAllocator{};
        this->buffer = NULL;
        this->length = 0;
    }
    inline string(IAllocator myAllocator)
    {
        this->allocator = myAllocator;
        this->buffer = NULL;
        this->length = 0;
    }
    inline string(IAllocator allocator, usize length)
    {
        this->allocator = allocator;
        this->buffer = (char*)allocator.Allocate(length);
        this->length = length;
    }
    inline string(IAllocator myAllocator, const char* source)
    {
        this->allocator = myAllocator;
        this->length = strlen(source) + 1;
        this->buffer = (char *)myAllocator.Allocate(this->length);
        strcpy(this->buffer, source);
        this->buffer[this->length - 1] = '\0';
    }
    inline string(IAllocator myAllocator, const char* source, usize length)
    {
        this->allocator = myAllocator;
        this->buffer = (char*)myAllocator.Allocate(length + 1);
        this->buffer[length] = '\0';
        this->length = length + 1;
        memcpy(this->buffer, source, length);
    }

    inline void deinit()
    {
        if (buffer != NULL)
        {
            this->allocator.Free(buffer);
            buffer = NULL;
            length = 0;
        }
    }
    inline bool eql(const char *other)
    {
        if (this->buffer == NULL || other == NULL)
        {
            return this->buffer == other;
        }
        return strcmp(buffer, other) == 0;
    }

    inline string *Prepend(const char *other)
    {
        usize otherLen = strlen(other);
        usize newLength = otherLen + this->length;
        if (this->length == 0)
        {
            newLength += 1;
        }
        char *newBuffer = (char*)this->allocator.Allocate(newLength);

        if (this->buffer != NULL)
        {
            memcpy(newBuffer, other, otherLen);
            memcpy(newBuffer + otherLen, this->buffer, this->length - 1);
        }
        else
        {
            memcpy(newBuffer, other, otherLen);
        }
        newBuffer[newLength - 1] = '\0';

        if (this->buffer != NULL)
        {
            this->allocator.FREEPTR(this->buffer);
        }
        this->buffer = newBuffer;
        this->length = newLength;
        return this;
    }
    inline string *Prepend(string other)
    {
        Prepend(other.buffer);
        return this;
    }
    inline string *Append(const char *other)
    {
        usize otherLen = strlen(other);
        usize newLength = otherLen + this->length;
        if (this->length == 0)
        {
            newLength += 1;
        }
        char *newBuffer = (char*)this->allocator.Allocate(newLength);

        if (this->buffer != NULL)
        {
            memcpy(newBuffer, this->buffer, this->length - 1);
            memcpy(newBuffer + this->length - 1, other, otherLen);
        }
        else
        {
            memcpy(newBuffer, other, otherLen);
        }
        newBuffer[newLength - 1] = '\0';

        if (this->buffer != NULL)
        {
            this->allocator.FREEPTR(this->buffer);
        }
        this->buffer = newBuffer;
        this->length = newLength;
        return this;
    }
    inline string *Append(i64 integer)
    {
        //max integer is 19 characters, with - sign its 20
        char chars[20];
        chars[19] = '\0';
        usize index = 19;
        i64 positive = integer < 0 ? -integer : integer;
        while (positive > 100)
        {
            index -= 2;
            memcpy(chars + index, digits2(positive % 100), 2);
            positive /= 100;
        }
        if (positive < 10)
        {
            index -= 1;
            chars[index] = '0' + positive;

            if (integer < 0)
            {
                index -= 1;
                *(chars + index) = '-';
            }
            
            return this->Append(chars + index);
        }
        index -= 2;
        memcpy(chars + index, digits2(positive), 2);
        if (integer < 0)
        {
            index -= 1;
            *(chars + index) = '-';
        }
        return this->Append(chars + index);
    }
    inline string *Append(u64 integer)
    {
        //max integer is 20 characters
        char chars[21];
        chars[20] = '\0';
        usize index = 20;
        while (integer > 100)
        {
            index -= 2;
            memcpy(chars + index, digits2(integer % 100), 2);
            integer /= 100;
        }
        if (integer < 10)
        {
            index -= 1;
            chars[index] = '0' + integer;

            return this->Append(chars + index);
        }
        index -= 2;
        memcpy(chars + index, digits2(integer), 2);
        this->Append(chars + index);
        return this;
    }
    inline string *Append(double value)
    {
        if (value == 0.0)
        {
            return this->Append("0.0");
        }
        char chars[16];
        i32 result = snprintf(chars, 16, "%lf", value);
        if (result >= 16)
        {
            result = 16;
        }
        chars[result - 1] = '\0';
        this->Append(chars);
        return this;
    }
    inline string *Append(float value)
    {
        if (value == 0.0f)
        {
            return this->Append("0.0");
        }
        char chars[16];
        i32 result = snprintf(chars, 16, "%f", value);
        if (result >= 16)
        {
            result = 16;
        }
        chars[result - 1] = '\0';
        this->Append(chars);
        return this;
    }

    inline string *PrependDeinit(string other)
    {
        this->Prepend(other.buffer);
        other.deinit();
        return this;
    }
    inline string *AppendDeinit(string other)
    {
        this->Append(other.buffer);
        other.deinit();
        return this;
    }

    inline string TrimStart(usize trimLength)
    {
        if (trimLength >= length)
        {
            deinit();
        }
        else
        {
            char *newBuffer = (char*)allocator.Allocate(length - trimLength);
            memcpy(newBuffer, buffer + trimLength, length - trimLength);
            newBuffer[length - trimLength - 1] = '\0';
            allocator.Free(buffer);
            buffer = newBuffer;
            length -= trimLength;
        }
        return *this;
    }
    inline string CloneTrimStart(IAllocator allocator, usize trimLength)
    {
        if (trimLength >= length)
        {
            return string(allocator);
        }
        else
        {
            char *newBuffer = (char*)allocator.Allocate(length - trimLength);
            memcpy(newBuffer, buffer + trimLength, length - trimLength);
            newBuffer[length - trimLength - 1] = '\0';

            string result;
            result.allocator = allocator;
            result.buffer = newBuffer;
            result.length = length - trimLength;
            return result;
        }
    }

    inline string CloneDeinit(IAllocator allocator)
    {
        string result = string(allocator, this->buffer);
        this->deinit();
        return result;
    }
    inline string Clone(IAllocator allocator)
    {
        return string(allocator, this->buffer, this->length - 1);
    }
    inline wchar_t* ToWString(IAllocator allocator)
    {
        wchar_t *result = (wchar_t *)allocator.Allocate(sizeof(wchar_t) * length);
        swprintf(result, length, L"%hs", buffer);
        result[length - 1] = L'\0';
        return result;
    }
    inline char_t* ToOSString(IAllocator allocator)
    {
#ifdef WINDOWS
        return ToWString(allocator);
#else
        return Clone(allocator).buffer;
#endif
    }
    inline bool StartsWith(const char* other)
    {
        if (this->buffer == NULL || other == NULL)
        {
            if (this->buffer == other)
            {
                return true;
            }
            return false;
        }
        usize len = strlen(other);
        if (len > length)
        {
            return false;
        }
        return memcmp(this->buffer, other, len) == 0;
    }
    inline bool EndsWith(const char* other)
    {
        if (this->buffer == NULL || other == NULL)
        {
            if (this->buffer == other)
            {
                return true;
            }
            return false;
        }
        usize len = strlen(other) + 1;
        if (this->length >= len)
        {
            return strcmp(this->buffer + this->length - len, other) == 0;
        }
        return false;
    }

    inline bool operator==(const char* other)
    {
        if (this->buffer == NULL || other == NULL)
        {
            return this->buffer == other;
        }
        return strcmp(this->buffer, other) == 0;
    }
    inline bool operator!=(const char* other)
    {
        if (this->buffer == NULL || other == NULL)
        {
            return this->buffer != other;
        }
        return strcmp(this->buffer, other) != 0;
    }
    inline bool operator==(string other)
    {
        if (this->buffer == NULL || other == NULL)
        {
            return this->buffer == other.buffer;
        }
        return strcmp(this->buffer, other.buffer) == 0;
    }
    inline bool operator!=(string other)
    {
        if (this->buffer == NULL || other == NULL)
        {
            return this->buffer != other.buffer;
        }
        return strcmp(this->buffer, other.buffer) != 0;
    }
    inline string operator+(string other)
    {
        string newString = Clone(allocator);
        newString.Append(other.buffer);
        return newString;
    }
    inline string operator+=(string other)
    {
        this->Append(other.buffer);
        return *this;
    }
    inline string operator+(text other)
    {
        string newString = Clone(allocator);
        newString.Append(other);
        return newString;
    }
    inline string operator+=(text other)
    {
        this->Append(other);
        return *this;
    }
};

struct CharSlice
{
    const char* buffer;
    usize length;

    inline CharSlice(string str)
    {
        buffer = str.buffer;
        length = str.length;
    }
    inline CharSlice(const char* stringLiteral)
    {
        buffer = stringLiteral;
        length = strlen(stringLiteral);
    }
    inline CharSlice(const char* stringLiteral, usize literalLength)
    {
        buffer = stringLiteral;
        length = literalLength;
    }
    inline bool operator==(text str)
    {
        //use memcmp not strcmp as CharSlice is probably not null terminated
        return memcmp(buffer, str, length) == 0;
    }
    inline bool operator!=(text str)
    {
        return memcmp(buffer, str, length) != 0;
    }
};

inline bool stringEql(string A, string B)
{
    if (A.buffer == NULL || B.buffer == NULL)
    {
        return A.buffer == B.buffer;
    }
    return strcmp(A.buffer, B.buffer) == 0;
}

inline u32 stringHash(string A)
{
    if (A.buffer == NULL)
    {
        return 7;
    }
    u32 hash = 7;
    for (usize i = 0; i < A.length - 1; i++)
    {
        hash = hash * 31 + A.buffer[i];
    }

    return hash;
}

inline u32 charHash(const char *A)
{
    u32 hash = 7;
    usize i = 0;
    while (true)
    {
        if (A[i] == '\0')
        {
            break;
        }
        else
        {
            hash = hash * 31 + A[i];
        }
        i += 1;
    }
    return hash;
}

inline option<usize> FindFirst(const char *buffer, char character)
{
    usize i = 0;
    while (buffer[i] != '\0' || character == '\0')
    {
        if (buffer[i] == character)
        {
            return option<usize>(i);
        }
        i++;
    }

    return option<usize>();
}

inline option<usize> FindLast(const char *buffer, char character)
{
    option<usize> result = option<usize>();
    usize i = 0;

    while (buffer[i] != '\0' || character == '\0')
    {
        if (buffer[i] == character)
        {
            result.value = i;
            result.present = true;
        }
        i++;
    }

    return result;
}

inline string ReplaceChar(IAllocator allocator, const char* input, char toReplace, char replaceWith)
{
    usize inputLength = strlen(input) + 1;
    string str = string(allocator);
    char* buffer = (char*)allocator.Allocate(inputLength);
    str.length = inputLength;

    for (usize i = 0; i < inputLength - 1; i++)
    {
        if (input[i] == toReplace)
        {
            buffer[i] = replaceWith;
        }
        else buffer[i] = input[i];
    }
    
    buffer[inputLength - 1] = '\0';

    str.buffer = buffer;
    return str;
}
inline string ReplaceCharWithString(IAllocator allocator, const char* input, char toReplace, const char* replaceWith)
{
    usize replaceWithLength = strlen(replaceWith);
    if (replaceWithLength == 1)
    {
        return ReplaceChar(allocator, input, toReplace, replaceWith[0]);
    }
    usize inputLength = strlen(input) + 1;
    usize lengthDiff = replaceWithLength - 1;
    usize outputLength = inputLength;

    for (usize i = 0; i < inputLength - 1; i++)
    {
        if (input[i] == toReplace)
        {
            outputLength += lengthDiff;
        }
    }

    string str = string(allocator);
    char* buffer = (char*)allocator.Allocate(outputLength);
    str.length = outputLength;

    usize at = 0;
    for (usize i = 0; i < inputLength; i++)
    {
        if (input[i] == toReplace)
        {
            strcpy(&buffer[at], replaceWith);
            at += replaceWithLength;
            //buffer[i] = toReplace;
        }
        else 
        {
            buffer[at] = input[i];
            at += 1;
        }
    }
    
    buffer[outputLength - 1] = '\0';

    str.buffer = buffer;
    return str;
}

inline collections::Array<string> SplitString(IAllocator allocator, const char* input, char toSplitOn)
{
    IAllocator defaultAllocator = GetCAllocator();
    collections::vector<string> results = collections::vector<string>(defaultAllocator);

    usize lastIndex = 0;
    usize i = 0;
    while (true)
    {
        if (input[i] == toSplitOn || input[i] == '\0')
        {
            if (lastIndex < i)
            {
                string element = string(allocator, input + lastIndex, i - lastIndex);
                results.Add(element);
                lastIndex = i + 1;
            }
        }
        if (input[i] == '\0')
        {
            break;
        }
        i += 1;
    }

    return results.ToOwnedArrayWith(allocator);
}

inline string ConcatFromCharSlices(IAllocator allocator, CharSlice* strings, usize length)
{
    usize totalLength = 1; //1 to account for the null termination of the concatenated string
    for (usize i = 0; i < length; i++)
    {
        if (strings[i].length > 0)
        {
            totalLength += strings[i].length;
            if (strings[i].buffer[strings[i].length - 1] == '\0')
            {
                totalLength -= 1;
            }
        }
    }
    char *buffer = (char *)allocator.Allocate(totalLength);
    usize index = 0;
    for (usize i = 0; i < length; i++)
    {
        if (strings[i].length > 0)
        {
            usize currentStringLength = strings[i].length;
            //if the string character is null terminated, remove the null termination before copying
            if (strings[i].buffer[currentStringLength - 1] == '\0')
            {
                currentStringLength -= 1;
            }
            memcpy(buffer + index, strings[i].buffer, currentStringLength);
            index += currentStringLength;
        }
    }
    //add the null termination to our new string
    buffer[totalLength - 1] = '\0';
    string result = string(allocator);
    result.buffer = buffer;
    result.length = totalLength;
    return result;
}