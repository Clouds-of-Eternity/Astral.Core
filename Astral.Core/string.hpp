#pragma once
#include "Allocators.hpp"
#include "string.h"
#include "option.hpp"
#include "Array.hpp"
#include "stdarg.h"
#include "stdio.h"
#include "math.h"
#include "List.hpp"
#include <wchar.h>

#ifndef INCL_STRING
#define INCL_STRING
#endif

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

struct CharSlice;

inline i64 TextToI64(const char* buffer, usize length)
{
    i64 result = 0;
    i64 exponent = 0;
    bool hasExponent = false;
    u32 index = 1;
    for (i32 i = (i32)length - 1; i >= 0; i--)
    {
        if (hasExponent)
        {
            i64 amount = index * (buffer[i] - (i64)'0');
            exponent += amount;
            index *= 10;
        }
        else if (buffer[i] == 'e' || buffer[i] == 'E')
        {
            hasExponent = true;
            index = 1;
        }
        else if (buffer[i] >= '0' && buffer[i] <= '9')
        {
            i64 amount = index * (buffer[i] - (i64)'0');
            result += amount;
            index *= 10;
        }
    }
    if (hasExponent)
    {
        i64 temp = exponent;
        exponent = result;
        result = temp;

        if (buffer[0] == '-')
        {
            result *= -1;
        }

        //if exponent 10, add 10 zeroes to the back
        i64 exponentMult = 1;
        for (u32 i = 0; i < exponent; i++)
        {
            exponentMult *= 10;
        }
        result *= exponentMult;
    }
    else
    {
        if (buffer[0] == '-')
        {
            result *= -1;
        }
    }
    return result;
}
inline u64 TextToU64(const char* buffer, usize length)
{
    u64 result = 0;
    u32 index = 1;
    u64 exponent = 0;
    bool hasExponent = false;
    for (i32 i = (i32)length - 1; i >= 0; i--)
    {
        if (hasExponent)
        {
            i64 amount = index * (buffer[i] - (i64)'0');
            exponent += amount;
            index *= 10;
        }
        else if (buffer[i] == 'e' || buffer[i] == 'E')
        {
            hasExponent = true;
            index = 1;
        }
        else if (buffer[i] >= '0' && buffer[i] <= '9')
        {
            i64 amount = index * (buffer[i] - (u64)'0');
            result += amount;
            index *= 10;
        }
    }
    if (hasExponent)
    {
        u64 temp = exponent;
        exponent = result;
        result = temp;
        //if exponent 10, add 10 zeroes to the back
        u64 exponentMult = 1;
        for (u32 i = 0; i < exponent; i++)
        {
            exponentMult *= 10;
        }
        result *= exponentMult;
    }
    return result;
}
inline bool IsLineBreak(u32 codepoint)
{
    return codepoint == '\n' || codepoint == '\r' || codepoint == '\v' || codepoint == '\f' || codepoint == 8232 || codepoint == 8233;
}
inline bool IsWhitespace(u32 codepoint)
{
    return codepoint == '\t' || codepoint == ' ' || codepoint == 160 || (codepoint >= 8192 && codepoint <= 8202) || codepoint == 8239 || codepoint == 8287 || codepoint == 12288;
}
inline bool IsWhitespaceOrLinebreak(u32 codepoint)
{
    return IsWhitespace(codepoint) || IsLineBreak(codepoint);
}

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
    inline char &operator[](usize index)
    {
        return buffer[index];
    }
    inline string(IAllocator myAllocator, const char* source)
    {
        this->allocator = myAllocator;
        if (source != NULL)
        {
            this->length = strlen(source) + 1;
            this->buffer = (char *)myAllocator.Allocate(this->length);
            memcpy(this->buffer, source, this->length - 1);
            this->buffer[this->length - 1] = '\0';
        }
        else
        {
            this->length = 0;
            this->buffer = NULL;
        }
    }
    inline string(IAllocator myAllocator, const char* source, usize length)
    {
        this->allocator = myAllocator;
        if (length == 0 || source == NULL)
        {
            buffer = NULL;
            this->length = 0;
        }
        else
        {
            this->buffer = (char*)myAllocator.Allocate(length + 1);
            this->buffer[length] = '\0';
            this->length = length + 1;
            memcpy(this->buffer, source, length);
        }
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
    inline string Clone(IAllocator allocator) const
    {
        return string(allocator, this->buffer, this->length - 1);
    }
    inline wchar_t* ToWString(IAllocator allocator) const
    {
        wchar_t *result = (wchar_t *)allocator.Allocate(sizeof(wchar_t) * length);
        swprintf(result, length, L"%hs", buffer);
        result[length - 1] = L'\0';
        return result;
    }
    inline char_t* ToOSString(IAllocator allocator) const
    {
#ifdef WINDOWS
        return ToWString(allocator);
#else
        return Clone(allocator).buffer;
#endif
    }
    inline bool StartsWith(CharSlice other) const;
    inline bool EndsWith(CharSlice other) const;

    inline bool operator==(text other) const
    {
        if (this->buffer == NULL || other == NULL)
        {
            return this->buffer == other;
        }
        return strcmp(this->buffer, other) == 0;
    }
    inline bool operator!=(text other) const
    {
        if (this->buffer == NULL || other == NULL)
        {
            return this->buffer != other;
        }
        return strcmp(this->buffer, other) != 0;
    }
    inline bool operator==(const string other) const
    {
        if (this->buffer == NULL || other == NULL)
        {
            return this->buffer == other.buffer;
        }
        return strcmp(this->buffer, other.buffer) == 0;
    }
    inline bool operator!=(const string other) const
    {
        if (this->buffer == NULL || other == NULL)
        {
            return this->buffer != other.buffer;
        }
        return strcmp(this->buffer, other.buffer) != 0;
    }
    bool operator==(const CharSlice other) const;
    bool operator!=(const CharSlice other) const;

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
    inline static string Format(IAllocator allocator, const char *input, ...)
    {
        va_list args;
        va_start(args, input);
        i32 requiredBytes = vsnprintf(NULL, 0, input, args);
        requiredBytes += 1;

        char *buffer = (char *)allocator.Allocate(requiredBytes);
        vsnprintf(buffer, requiredBytes, input, args);
        buffer[requiredBytes - 1] = '\0';

        va_end(args);
        string result = string();
        result.allocator = allocator;
        result.buffer = buffer;
        result.length = requiredBytes;

        return result;
    }

    inline i64 ToI64()
    {
        return TextToI64(buffer, length - 1);
    }
    inline u64 ToU64()
    {
        return TextToU64(buffer, length - 1);
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

struct CharSlice
{
    const char* buffer;
    usize length;

    inline CharSlice()
    {
        buffer = NULL;
        length = 0;
    }
    inline CharSlice(string str)
    {
        buffer = str.buffer;
        if (str.length > 0)
        {
            length = str.length - 1;
        }
        else
        {
            length = 0;
        }
    }
    inline CharSlice(const char* stringLiteral)
    {
        buffer = stringLiteral;
        if (stringLiteral == NULL)
        {
            length = 0;
        }
        else length = strlen(stringLiteral);
    }
    inline CharSlice(const char* stringLiteral, usize literalLength)
    {
        buffer = stringLiteral;
        length = literalLength;
    }
    inline bool operator==(text str) const
    {
        if (str == NULL)
        {
            return buffer == NULL;
        }
        return memcmp(buffer, str, length) == 0;
    }
    inline bool operator!=(text str) const
    {
        if (str == buffer)
        {
            return false;
        }
        return str == NULL || buffer == NULL || memcmp(buffer, str, length) != 0;
    }
    inline bool operator==(const CharSlice str) const
    {
        if (str.buffer == NULL)
        {
            return buffer == NULL;
        }
        if (str.length != length)
        {
            return false;
        }
        return memcmp(str.buffer, buffer, length) == 0;
    }
    inline bool operator!=(const CharSlice str) const
    {
        return !(*this == str);
    }
    inline char operator[](usize index) const
    {
        assert(index < length);
        return buffer[index];
    }
    inline CharSlice Slice(u32 startIndex) const
    {
        if (startIndex >= length)
        {
            return CharSlice();
        }
        return CharSlice(buffer + startIndex, this->length - startIndex);
    }
    inline CharSlice Slice(u32 startIndex, u32 sliceLength) const
    {
        if (startIndex + sliceLength > this->length)
        {
            return CharSlice();
        }
        return CharSlice(buffer + startIndex, sliceLength);
    }
    
    inline string Slice(IAllocator allocator, u32 startIndex) const
    {
        if (startIndex >= length)
        {
            return string();
        }
        return string(allocator, buffer + startIndex, this->length - startIndex);
    }
    inline string Slice(IAllocator allocator, u32 startIndex, u32 sliceLength) const
    {
        if (startIndex + sliceLength > this->length)
        {
            return string();
        }
        return string(allocator, buffer + startIndex, sliceLength);
    }

    inline CharSlice TrimStartWhitespace() const
    {
        CharSlice result = *this;

        while (result.length > 0)
        {
            if (IsWhitespaceOrLinebreak(result.buffer[0]))
            {
                result.buffer++;
                result.length--;
            }
            else break;
        }
        return result;
    }
    inline CharSlice TrimEndWhitespace() const
    {
        CharSlice result = *this;

        while (result.length > 0)
        {
            if (IsWhitespaceOrLinebreak(result.buffer[result.length - 1]))
            {
                result.length--;
            }
            else break;
        }
        return result;
    }
    inline CharSlice TrimStartAndEndWhitespaces() const
    {
        CharSlice result = TrimStartWhitespace();
        return result.TrimEndWhitespace();
    }

    inline void CopyTo(char *output, bool addNullTerminator) const
    {
        memcpy(output, buffer, length);
        if (addNullTerminator)
        {
            output[length] = '\0';
        }
    }

    inline bool StartsWith(CharSlice other) const
    {
        if (this->buffer == NULL || other == NULL)
        {
            if (this->buffer == other.buffer)
            {
                return true;
            }
            return false;
        }
        const usize otherLength = other.length;
        if (otherLength > length)
        {
            return false;
        }
        return memcmp(this->buffer, other.buffer, otherLength) == 0;
    }
    inline bool EndsWith(CharSlice other) const
    {
        if (this->buffer == NULL || other == NULL)
        {
            if (this->buffer == other.buffer)
            {
                return true;
            }
            return false;
        }
        const usize otherLength = other.length;
        if (length >= otherLength)
        {
            return memcmp(this->buffer + this->length - otherLength, other.buffer, otherLength) == 0;
        }
        return false;
    }
    inline bool FindFirst(char character, usize *outputIndex)
    {
        for (usize i = 0; i <= length; i++)
        {
            if (buffer[i] == character)
            {
                *outputIndex = i;
                return true;
            }
        }
        return false;
    }
    inline bool FindLast(char character, usize *outputIndex)
    {
        for (i64 i = length - 1; i >= 0; i--)
        {
            if (buffer[i] == character)
            {
                *outputIndex = i;
                return true;
            }
        }
        return false;
    }

    inline u64 ToU64() const
    {
        return TextToU64(buffer, length);
    }

    inline i64 ToI64() const
    {
        return TextToI64(buffer, length);
    }

    inline string ToString(IAllocator allocator) const
    {
        return string(allocator, buffer, length);
    }
};
inline bool string::operator==(const CharSlice other) const
{
    return other == *this;
}
inline bool string::operator!=(const CharSlice other) const
{
    return other != *this;
}
inline bool string::StartsWith(CharSlice other) const
{
    if (this->buffer == NULL || other.buffer == NULL)
    {
        if (this->buffer == other.buffer)
        {
            return true;
        }
        return false;
    }
    usize otherLength = other.length;
    //>= since charslices do not count null terminators, but strings do.
    if (otherLength >= length)
    {
        return false;
    }
    return memcmp(this->buffer, other.buffer, otherLength) == 0;
}
inline bool string::EndsWith(CharSlice other) const
{
    if (this->buffer == NULL || other.buffer == NULL)
    {
        if (this->buffer == other.buffer)
        {
            return true;
        }
        return false;
    }
    usize otherLength = other.length;
    if (this->length > otherLength)
    {
        return memcmp(this->buffer + this->length - 1 - otherLength, other.buffer, other.length) == 0;
    }
    return false;
}

inline bool CharSliceEql(CharSlice A, CharSlice B)
{
    if (A.buffer == NULL || B.buffer == NULL)
    {
        return A.buffer == B.buffer;
    }
    if (A.length != B.length)
    {
        return false;
    }
    return memcmp(A.buffer, B.buffer, A.length) == 0;
}

inline u32 CharSliceHash(CharSlice A)
{
    if (A.buffer == NULL)
    {
        return 7;
    }
    u32 hash = 7;
    for (usize i = 0; i < A.length; i++)
    {
        hash = hash * 31 + A.buffer[i];
    }

    return hash;
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

inline string ReplaceChar(IAllocator allocator, CharSlice input, char toReplace, char replaceWith)
{
    string str = string(allocator);
    char* buffer = (char*)allocator.Allocate(input.length + 1);

    usize index = 0;
    for (usize i = 0; i < input.length; i++)
    {
        if (input.buffer[i] == toReplace)
        {
            if (replaceWith != '\0')
            {
                buffer[index++] = replaceWith;
            }
        }
        else buffer[index++] = input.buffer[i];
    }
    
    buffer[index++] = '\0';
    str.length = index;

    str.buffer = buffer;
    return str;
}
inline string ReplaceCharWithString(IAllocator allocator, CharSlice input, char toReplace, const char* replaceWith)
{
    usize replaceWithLength = strlen(replaceWith);
    if (replaceWithLength == 1)
    {
        return ReplaceChar(allocator, input, toReplace, replaceWith[0]);
    }
    usize lengthDiff = replaceWithLength - 1;
    usize outputLength = input.length + 1;

    for (usize i = 0; i < input.length; i++)
    {
        if (input.buffer[i] == toReplace)
        {
            outputLength += lengthDiff;
        }
    }

    string str = string(allocator);
    char* buffer = (char*)allocator.Allocate(outputLength);
    str.length = outputLength;

    usize at = 0;
    for (usize i = 0; i < input.length; i++)
    {
        if (input.buffer[i] == toReplace)
        {
            strcpy(&buffer[at], replaceWith);
            at += replaceWithLength;
            //buffer[i] = toReplace;
        }
        else 
        {
            buffer[at] = input.buffer[i];
            at += 1;
        }
    }
    
    buffer[outputLength - 1] = '\0';

    str.buffer = buffer;
    return str;
}
inline string ReplaceStringWithChar(IAllocator allocator, CharSlice input, const char *toReplace, char replaceWith)
{
    usize toReplaceLength = strlen(toReplace);
    if (toReplaceLength == 1)
    {
        return ReplaceChar(allocator, input, toReplace[0], replaceWith);
    }
    usize outputLength = input.length;

    for (usize i = 0; i < input.length; i++)
    {
        if (i < input.length - toReplaceLength)
        {
            if (memcmp(input.buffer + i, toReplace, toReplaceLength) == 0)
            {
                outputLength -= toReplaceLength - 1;
            }
        }
    }

    string str = string(allocator);
    char* buffer = (char*)allocator.Allocate(outputLength);
    str.length = outputLength;

    usize at = 0;
    for (usize i = 0; i < input.length; i++)
    {
        if (memcmp(input.buffer + i, toReplace, toReplaceLength) == 0)
        {
            buffer[at] = replaceWith;
            i += toReplaceLength - 1;
        }
        else 
        {
            buffer[at] = input.buffer[i];
        }
        at++;
    }
    return str;
}

inline collections::Array<string> SplitString(IAllocator allocator, const char* input, char toSplitOn)
{
    IAllocator defaultAllocator = GetCAllocator();
    collections::List<string> results = collections::List<string>(defaultAllocator);

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

inline bool TextIsUint(CharSlice str, bool allowLeadingZeroes)
{
    bool foundOtherThanZero = false;
    for (u32 i = 0; i < str.length; i++)
    {
        if (str.buffer[i] == '0')
        {
            if (!foundOtherThanZero && !allowLeadingZeroes)
            {
                return false;
            }
        }
        else if (str.buffer[i] < '1' || str.buffer[i] > '9')
        {
            return false;
        }
    }
    return true;
}
inline bool TextIsInt(CharSlice str)
{
    if (str.length == 0)
    {
        return false;
    }
    if (str.buffer[0] == '-')
    {
        str.buffer++;
        str.length--;
    }
    return TextIsUint(str, false);
}