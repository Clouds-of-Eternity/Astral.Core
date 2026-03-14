#pragma once
#include "Array.h"
#include "List.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef WINDOWS
typedef wchar_t char_t;
#else
typedef char char_t;
#endif

typedef struct
{
    IAllocator allocator;
    char *buffer;
    size_t length;
} string;

inline string StringEmpty()
{
    const string result = {};
    return result;
}
inline string StringFrom(IAllocator allocator, const char *input)
{
    const size_t len = strlen(input) + 1;
    char *buffer = (char *)IAllocator_Allocate(allocator, len);
    memcpy(buffer, input, len);
    string result = {allocator, buffer, len};

    return result;
}
inline string StringFromSlice(IAllocator allocator, const char *input, size_t length)
{
    char *buffer = (char *)IAllocator_Allocate(allocator, length + 1);
    memcpy(buffer, input, length);
    buffer[length] = '\0';
    string result = {allocator, buffer, length + 1};

    return result;
}
inline string StringFromLength(IAllocator allocator, size_t lengthNoNullTerminator)
{
    char *buffer = (char *)IAllocator_Allocate(allocator, lengthNoNullTerminator);
    buffer[lengthNoNullTerminator] = '\0';
    string result = {allocator, buffer, lengthNoNullTerminator + 1};
    
    return result;
}
inline void String_Deinit(string *self)
{
    if (self->buffer != NULL)
    IAllocator_Free(self->allocator, self->buffer);
}

inline string String_AppendText(string self, const char *toAppend)
{
    const size_t toAppendLen = strlen(toAppend);
    const size_t newStrLen = self.length + toAppendLen;
    char *buffer = IAllocator_Allocate(self.allocator, newStrLen);

    size_t offset = 0;
    if (self.length > 1)
    {
        offset = self.length - 1;
        memcpy(buffer, self.buffer, offset);
    }
    memcpy(buffer + offset, toAppend, toAppendLen + 1);

    string result = {self.allocator, buffer, newStrLen};
    return result;
}
inline string String_PrependText(string self, const char *toPrepend)
{
    const size_t toPrependLen = strlen(toPrepend);
    const size_t newStrLen = self.length + toPrependLen;
    char *buffer = IAllocator_Allocate(self.allocator, newStrLen);

    if (self.length > 1)
    {
        memcpy(buffer + toPrependLen, self.buffer, self.length);
    }
    memcpy(buffer, toPrepend, toPrependLen);

    string result = {self.allocator, buffer, newStrLen};
    return result;
}
inline string String_AppendStr(string self, const string toAppend)
{
    return String_AppendText(self, toAppend.buffer);
}
inline string String_PrependStr(string self, const string toPrepend)
{
    return String_PrependText(self, toPrepend.buffer);
}

inline string StringFormat(IAllocator allocator, const char *input, ...)
{
    va_list args;
    va_start(args, input);
    int32_t requiredBytes = vsnprintf(NULL, 0, input, args);
    requiredBytes += 1;

    char *buffer = IAllocator_Allocate(allocator, requiredBytes);
    vsnprintf(buffer, requiredBytes, input, args);

    va_end(args);

    string result = {allocator, buffer, (size_t)requiredBytes};
    return result;
}
inline string StringClone(IAllocator newAllocator, const string str)
{
    return StringFrom(newAllocator, str.buffer);
}
inline bool StringStartsWith(string self, const char* other)
{
    if (self.buffer == NULL || other == NULL)
    {
        if (self.buffer == other)
        {
            return true;
        }
        return false;
    }
    size_t len = strlen(other);
    if (len >= self.length)
    {
        return false;
    }
    return memcmp(self.buffer, other, len) == 0;
}
inline bool StringEndsWith(string self, const char* other)
{
    if (self.buffer == NULL || other == NULL)
    {
        if (self.buffer == other)
        {
            return true;
        }
        return false;
    }
    size_t len = strlen(other) + 1;
    if (self.length >= len)
    {
        return strcmp(self.buffer + self.length - len, other) == 0;
    }
    return false;
}
inline bool StringEqls(string A, string B)
{
    return A.length == B.length && memcmp(A.buffer, B.buffer, A.length) == 0;
}


inline wchar_t* StringToWChar(string self, IAllocator allocator)
{
    wchar_t *result = (wchar_t *)IAllocator_Allocate(allocator, sizeof(wchar_t) * self.length);
    swprintf(result, self.length, L"%hs", self.buffer);
    result[self.length - 1] = L'\0';
    return result;
}
inline char_t* StringToOSChars(string self, IAllocator allocator)
{
#ifdef WINDOWS
    return StringToWChar(self, allocator);
#else
    return StringClone(self, allocator).buffer;
#endif
}

inline Array SplitStringOnChar(IAllocator allocator, const char* input, char toSplitOn)
{
    size_t count = 1;
    size_t i = 0;
    while (true)
    {
        if (input[i] == toSplitOn)
        {
            count++;
        }
        else if (input[i] == '\0')
        {
            break;
        }
    }
    Array results = ARRAY(string, allocator, count);

    size_t lastIndex = 0;
    count = 0;
    i = 0;
    while (true)
    {
        if (input[i] == toSplitOn || input[i] == '\0')
        {
            if (lastIndex < i)
            {
                string element = StringFromSlice(allocator, input + lastIndex, i - lastIndex);
                //List_Add(&results, &element);
                ARRAY_AS(results, string)[count] = element;

                count++;
                lastIndex = i + 1;
            }
        }
        if (input[i] == '\0')
        {
            break;
        }
        i += 1;
    }

    return results;
}
inline Array SplitStringOnAnyChar(IAllocator allocator, const char* input, const char *toSplitOn)
{
    size_t splitCharsCount = strlen(toSplitOn);

    size_t count = 1;
    size_t i = 0;
    while (true)
    {
        for (size_t j = 0; j < splitCharsCount; j++)
        {
            if (input[i] == toSplitOn[j])
            {
                count++;
                break;
            }
        }

        if (input[i] == '\0')
        {
            break;
        }
    }
    Array results = ARRAY(string, allocator, count);

    size_t lastIndex = 0;
    count = 0;
    i = 0;
    while (true)
    {
        bool isOnSplitChar = false;

        for (size_t j = 0; j < splitCharsCount; j++)
        {
            if (input[i] == toSplitOn[j])
            {
                isOnSplitChar = true;
                break;
            }
        }

        if (isOnSplitChar || input[i] == '\0')
        {
            if (lastIndex < i)
            {
                string element = StringFromSlice(allocator, input + lastIndex, i - lastIndex);
                //List_Add(&results, &element);
                ARRAY_AS(results, string)[count] = element;

                count++;
                lastIndex = i + 1;
            }
        }
        if (input[i] == '\0')
        {
            break;
        }
        i += 1;
    }

    return results;
}
inline string ReplaceChar(IAllocator allocator, const char* input, char toReplace, char replaceWith)
{
    size_t inputLength = strlen(input) + 1;

    char* buffer = (char*)IAllocator_Allocate(allocator, inputLength);

    size_t index = 0;
    for (size_t i = 0; i < inputLength - 1; i++)
    {
        if (input[i] == toReplace)
        {
            if (replaceWith != '\0')
            {
                buffer[index++] = replaceWith;
            }
        }
        else buffer[index++] = input[i];
    }
    
    buffer[index++] = '\0';
    string str;
    str.allocator = allocator;
    str.length = index;
    str.buffer = buffer;
    return str;
}
inline string ReplaceCharWithString(IAllocator allocator, const char* input, char toReplace, const char* replaceWith)
{
    size_t replaceWithLength = strlen(replaceWith);
    if (replaceWithLength == 1)
    {
        return ReplaceChar(allocator, input, toReplace, replaceWith[0]);
    }
    size_t inputLength = strlen(input) + 1;
    size_t lengthDiff = replaceWithLength - 1;
    size_t outputLength = inputLength;

    for (size_t i = 0; i < inputLength - 1; i++)
    {
        if (input[i] == toReplace)
        {
            outputLength += lengthDiff;
        }
    }

    char* buffer = (char*)IAllocator_Allocate(allocator, outputLength);
    string str;
    str.allocator = allocator;
    str.length = outputLength;

    size_t at = 0;
    for (size_t i = 0; i < inputLength; i++)
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

inline bool StringFindLast(string str, char character, size_t *outIndex)
{
    for (int64_t i = (int64_t)str.length - 1; i >= 0; i--)
    {
        if (str.buffer[i] == character)
        {
            *outIndex = (size_t)i;
            return true;
        }
    }
    return false;
}
inline bool StringFindAnyFromEnd(string str, const char *characters, size_t *outIndex)
{
    size_t charLen = strlen(characters);

    for (int64_t i = (int64_t)str.length - 1; i >= 0; i--)
    {
        for (size_t j = 0; j < charLen; j++)
        {
            if (str.buffer[i] == characters[j])
            {
                *outIndex = (size_t)i;
                return true;
            }
        }
    }
    return false;
}
inline bool StringFindFirst(string str, char character, size_t *outIndex)
{
    for (size_t i = 0; i < str.length; i++)
    {
        if (str.buffer[i] == character)
        {
            *outIndex = i;
            return true;
        }
    }
    return false;
}
inline bool StringFindAnyFromStart(string str, const char *characters, size_t *outIndex)
{
    size_t charLen = strlen(characters);

    for (size_t i = 0; i < str.length; i++)
    {
        for (size_t j = 0; j < charLen; j++)
        {
            if (str.buffer[i] == characters[j])
            {
                *outIndex = i;
                return true;
            }
        }
    }
    return false;
}

inline bool LitFindLast(const char *str, char character, size_t *outIndex)
{
    size_t len = strlen(str) + 1;
    for (int64_t i = (int64_t)len; i >= 0; i--)
    {
        if (str[i] == character)
        {
            *outIndex = (size_t)i;
            return true;
        }
    }
    return false;
}
inline bool LitFindFirst(const char *str, char character, size_t *outIndex)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        if (str[i] == character)
        {
            *outIndex = i;
            return true;
        }
    }
    return false;
}