#pragma once
#include <assert.h>
#include "Array.hpp"

template <typename T>
struct Span
{
    def_delegate(EqlFunc, bool, T& A, T& B);

    T* data;
    usize length;

    inline Span()
    {
        data = NULL;
        length = 0;
    }
    inline Span(T *input, usize length)
    {
        data = input;
        length = length;
    }
    inline Span(const collections::Array<T> input)
    {
        data = input.data;
        length = input.length;
    }
    inline Span(const collections::Array<T> input, usize startIndex)
    {
        if (startIndex >= input.length)
        {
            this->data = NULL;
            this->length = 0;
        }
        else
        {
            this->data = &input.data[startIndex];
            this->length = input.length - startIndex;
       }
    }
    inline Span(const collections::Array<T> input, usize startIndex, usize spanLength)
    {
        if (startIndex >= input.length)
        {
            this->data = NULL;
            this->length = 0;
        }
        else
        {
            this->data = &input.data[startIndex];
            if (startIndex + spanLength > input.length)
            {
                spanLength = input.length - startIndex;
            }
            this->length = spanLength;
       }
    }

    inline T& operator[](usize index)
    {
        assert(index < length);
        return data[index];
    }
    inline T Get(usize index) const
    {
        if (index < length)
        {
            return data[index];
        }
        return {};
    }
    inline Span Slice(usize startIndex) const
    {
        if (startIndex >= length)
        {
            return Span();
        }
        return Span(data + startIndex, length - startIndex);
    }
    inline Span Slice(usize startIndex, usize sliceLength) const
    {
        if (startIndex >= length)
        {
            return Span();
        }
        if (startIndex + sliceLength > this->length)
        {
            sliceLength = this->length - startIndex;
        }
        return Span(data + startIndex, sliceLength);
    }

    inline bool Equals(Span other, EqlFunc eqlFunc)
    {
        if (length != other.length)
        {
            return false;
        }
        for (u32 i = 0; i < length; i++)
        {
            if (!eqlFunc(data[i], other.data[i]))
            {
                return false;
            }
        }
        return true;
    }
};

template <typename T>
struct ReadOnlySpan
{
    def_delegate(EqlFunc, bool, T& A, T& B);

    const T* data;
    usize length;

    inline ReadOnlySpan()
    {
        data = NULL;
        length = 0;
    }
    inline ReadOnlySpan(T *input, usize length)
    {
        this->data = input;
        this->length = length;
    }
    inline ReadOnlySpan(Span<T> input)
    {
        this->data = input.data;
        this->length = input.length;
    }
    inline ReadOnlySpan(const collections::Array<T> input)
    {
        this->data = input.data;
        this->length = input.length;
    }
    inline ReadOnlySpan(const collections::Array<T> input, usize startIndex)
    {
        if (startIndex >= input.length)
        {
            this->data = NULL;
            this->length = 0;
        }
        else
        {
            this->data = &input.data[startIndex];
            this->length = input.length - startIndex;
       }
    }
    inline ReadOnlySpan(const collections::Array<T> input, usize startIndex, usize spanLength)
    {
        if (startIndex >= input.length)
        {
            this->data = NULL;
            this->length = 0;
        }
        else
        {
            this->data = &input.data[startIndex];
            if (startIndex + spanLength > input.length)
            {
                spanLength = input.length - startIndex;
            }
            this->length = spanLength;
       }
    }

    inline const T& operator[](usize index) const
    {
        assert(index < length);
        return data[index];
    }
    inline const T Get(usize index) const
    {
        if (index < length)
        {
            return data[index];
        }
        return {};
    }
    inline ReadOnlySpan<T> Slice(usize startIndex) const
    {
        if (startIndex >= length)
        {
            return ReadOnlySpan<T>();
        }
        return ReadOnlySpan<T>(data + startIndex, length - startIndex);
    }
    inline ReadOnlySpan<T> Slice(usize startIndex, usize sliceLength) const
    {
        if (startIndex >= length)
        {
            return ReadOnlySpan<T>();
        }
        if (startIndex + sliceLength > this->length)
        {
            sliceLength = this->length - startIndex;
        }
        return ReadOnlySpan<T>(data + startIndex, sliceLength);
    }

    inline bool Equals(ReadOnlySpan<T> other, EqlFunc eqlFunc)
    {
        if (length != other.length)
        {
            return false;
        }
        for (u32 i = 0; i < length; i++)
        {
            if (!eqlFunc(data[i], other.data[i]))
            {
                return false;
            }
        }
        return true;
    }
};