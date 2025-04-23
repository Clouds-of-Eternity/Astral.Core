#pragma once
#include "string.hpp"
#include "allocators.hpp"
#include "ctype.h"
#include "hashmap.hpp"
#include "io.hpp"
#include "stdio.h"
#include "Maths/Util.hpp"
#include "StringRental.hpp"
#include "ByteStreamOps.hpp"

namespace Json
{
    thread_local extern StringRentalBuffer *stringRentalBuffer;
    enum JsonTokenType
    {
        JsonToken_Invalid,
        JsonToken_StringLiteral,
        JsonToken_IntegerLiteral,
        JsonToken_UIntegerLiteral,
        JsonToken_FloatLiteral,
        JsonToken_BoolLiteral,
        JsonToken_NullLiteral,
        JsonToken_Colon,
        
        JsonToken_LBrace,
        JsonToken_RBrace,
        JsonToken_LBracket,
        JsonToken_RBracket,
        JsonToken_Comma,
        JsonToken_Eof,

        //only used by JsonWriter

        JsonToken_PropertyName
    };
    enum JsonElementType
    {
        JsonElement_Object,
        JsonElement_Array,
        JsonElement_Property
    };
    struct JsonToken
    {
        JsonTokenType tokenType;
        usize startIndex;
        usize endIndex;
    };
    struct JsonTokenizer
    {
        const char *fileContents;
        usize length;
        usize currentIndex;
        usize currentLine;

        inline JsonTokenizer(string contents)
        {
            this->fileContents = contents.buffer;
            this->length = contents.length;
            this->currentIndex = 0;
            this->currentLine = 0;
        }
        inline string GetString(IAllocator allocator, JsonToken token)
        {
            if (token.tokenType == JsonToken_StringLiteral)
            {
                return string(allocator, fileContents + token.startIndex + 1, token.endIndex - token.startIndex - 2);
            }
            return string(allocator, fileContents + token.startIndex, token.endIndex - token.startIndex);
        }
        inline string RentString(JsonToken token)
        {
            if (token.tokenType == JsonToken_StringLiteral)
            {
                return stringRentalBuffer->Rent(fileContents + token.startIndex + 1, token.endIndex - token.startIndex - 2);
            }
            return stringRentalBuffer->Rent(fileContents + token.startIndex, token.endIndex - token.startIndex);
        }
        inline JsonToken PeekNext()
        {
            usize initialLine = currentLine;
            usize initialIndex = currentIndex;
            JsonToken result = Next();
            currentIndex = initialIndex;
            currentLine = initialLine;
            return result;
        }
        JsonToken Next();
    };
    struct JsonElement
    {
        collections::hashmap<string, JsonElement> childObjects;
        collections::Array<JsonElement> arrayElements;
        u64 data;
        //negative numbers = typed data
        //positive numbers = length of string
        i32 dataLength;

        JsonElementType elementType;

        inline JsonElement()
        {
            data = 0;
            dataLength = 0;
            childObjects = collections::hashmap<string, JsonElement>();
            arrayElements = collections::Array<JsonElement>();
            elementType = JsonElement_Property;
        }
        inline JsonElement(string stringValue)
        {
            data = (u64)stringValue.buffer;
            dataLength = (i32)stringValue.length;
            elementType = JsonElement_Property;
            childObjects = collections::hashmap<string, JsonElement>();
            arrayElements = collections::Array<JsonElement>();
        }
        inline JsonElement(collections::hashmap<string, JsonElement> thisChildObjects)
        {
            this->childObjects = thisChildObjects;
            this->arrayElements = collections::Array<JsonElement>();
            elementType = JsonElement_Object;
            data = 0;
            dataLength = -(i32)JsonToken_LBrace;
        }
        inline JsonElement(collections::Array<JsonElement> childElements)
        {
            this->arrayElements = childElements;
            this->childObjects = collections::hashmap<string, JsonElement>();
            elementType = JsonElement_Array;
            data = 0;
            dataLength = -(i32)JsonToken_LBracket;
        }
        
        inline JsonTokenType CheckElementType()
        {
            if (dataLength < 0)
            {
                return (JsonTokenType)(-dataLength);
            }
            return JsonToken_StringLiteral;
        }
        inline i8 GetInt8()
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(i8 *)&data;
        }
        inline u8 GetUint8()
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(u8 *)&data;
        }
        inline i16 GetInt16()
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(i16 *)&data;
        }
        inline u16 GetUint16()
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(u16 *)&data;
        }
        inline i32 GetInt32()
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(i32 *)&data;
        }
        inline u32 GetUint32()
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(u32 *)&data;
        }
        inline i64 GetInt64()
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(i64 *)&data;
        }
        inline u64 GetUint64()
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return data;
        }
        inline bool GetBool()
        {
            if (elementType != JsonElement_Property)
            {
                return false;
            }
            return (bool)data;
        }
        inline float GetFloat()
        {
            if (elementType != JsonElement_Property)
            {
                return 0.0f;
            }
            if (dataLength == -(i32)JsonToken_IntegerLiteral)
            {
                return (float)*(i64 *)&data;
            }
            else if (dataLength == -(i32)JsonToken_UIntegerLiteral)
            {
                return (float)*(u64 *)&data;
            }
            double asDouble = *(double *)&data;
            return (float)asDouble;
        }
        inline double GetDouble()
        {
            if (elementType != JsonElement_Property)
            {
                return 0.0;
            }
            if (dataLength == -(i32)JsonToken_IntegerLiteral)
            {
                return (double)*(i64 *)&data;
            }
            else if (dataLength == -(i32)JsonToken_UIntegerLiteral)
            {
                return (double)*(u64 *)&data;
            }
            return *(double *)&data;
        }
        #ifdef VEC2_H
        inline Maths::Vec2 GetVec2()
        {
            if (elementType != JsonElement_Array)
            {
                return Maths::Vec2(0.0f);
            }
            Maths::Vec2 result = Maths::Vec2(arrayElements.data[0].GetFloat(), arrayElements.data[1].GetFloat());
            return result;
        }
        #endif
        #ifdef VEC3_H
        inline Maths::Vec3 GetVec3()
        {
            if (elementType != JsonElement_Array)
            {
                return Maths::Vec3(0.0f);
            }
            Maths::Vec3 result = Maths::Vec3(arrayElements.data[0].GetFloat(), arrayElements.data[1].GetFloat(), arrayElements.data[2].GetFloat());
            return result;
        }
        #endif
        #ifdef VEC4_H
        inline Maths::Vec4 GetVec4()
        {
            if (elementType != JsonElement_Array)
            {
                return Maths::Vec4(0.0f);
            }
            Maths::Vec4 result = Maths::Vec4(arrayElements.data[0].GetFloat(), arrayElements.data[1].GetFloat(), arrayElements.data[2].GetFloat(), arrayElements.data[3].GetFloat());
            return result;
        }
        #endif
        inline string GetString(IAllocator allocator)
        {
            if (dataLength < 0)
            {
                return string();
            }
            collections::vector<CharSlice> charSlices = collections::vector<CharSlice>(GetCAllocator());
            usize start = 0;
            char *buffer = (char *)data;
            for (usize i = 0; i < dataLength - 1; i++)
            {
                if (buffer[i] == '\\' && i < dataLength - 2)
                {
                    if (buffer[i + 1] == '\"')
                    {
                        charSlices.Add(CharSlice(buffer + start, i - start));
                        charSlices.Add(CharSlice("\""));
                        i++;
                        start = i + 1;
                    }
                    else if (buffer[i + 1] == 'n')
                    {
                        charSlices.Add(CharSlice(buffer + start, i - start));
                        charSlices.Add(CharSlice("\n"));
                        i++;
                        start = i + 1;
                    }
                }
            }
            if (start < dataLength - 1)
            {
                charSlices.Add(CharSlice(buffer + start, dataLength - 1 - start));
            }
            string result = ConcatFromCharSlices(allocator, charSlices.ptr, charSlices.count);
            charSlices.deinit();
            return result;
        }
        inline string GetStringRaw(IAllocator allocator)
        {
            if (dataLength < 0)
            {
                return string();
            }
            return string(allocator, (char *)data);
        }
        inline string AsString()
        {
            if (dataLength <= 0)
            {
                return string();
            }
            string str = string();
            str.buffer = (char *)data;
            str.length = dataLength;
            return str;
        }
        template<typename T>
        inline T GetBytesAsT()
        {
            T result = T(); //invoke default constructor
            u8 *asPtr = (u8*)&result;
            for (usize i = 0; i < this->arrayElements.length; i++)
            {
                u8 byte = (u8)this->arrayElements.data[i].GetUint32();
                asPtr[i] = byte;
            }
            return result;
        }
        inline JsonElement *GetProperty(string propertyName)
        {
            if (this->elementType == JsonElement_Object)
            {
                return childObjects.Get(propertyName);
            }
            return NULL;
        }
        inline JsonElement *GetProperty(const char* ptr)
        {
            if (this->elementType == JsonElement_Object)
            {
                IAllocator allocator = GetCAllocator();
                JsonElement *result;
                if (stringRentalBuffer == NULL)
                {
                    string propertyName = string(allocator, ptr);
                    result = childObjects.Get(propertyName);
                    propertyName.deinit();
                }
                else
                {
                    string propertyName = stringRentalBuffer->Rent(ptr);
                    result = childObjects.Get(propertyName);
                    stringRentalBuffer->Return(propertyName);
                }
                return result;
            }
            return NULL;
        }
        /// @brief Parses all child objects recursively, collecting their properties 
        /// and aggregating them into raw byte data
        /// @return the Json object as raw data
        collections::Array<u8> GetAsRawData(IAllocator allocator);
    };
    bool ParseJsonElement(IAllocator allocator, JsonTokenizer *tokenizer, JsonElement *result);
    inline usize ParseJsonDocument(IAllocator allocator, string contents, JsonElement* result)
    {
        JsonTokenizer tokenizer = JsonTokenizer(contents);
        if (!ParseJsonElement(allocator, &tokenizer, result))
        {
            return tokenizer.currentLine;
        }
        return 0;
    }
    inline usize ReadJsonFile(IAllocator allocator, string fileFullPath, JsonElement* result)
    {
        IAllocator cAllocator = GetCAllocator();
        string fileContents = io::ReadFile(cAllocator, fileFullPath.buffer, false);
        if (fileContents.buffer == NULL)
        {
            return false;
        }

        usize parseResult = ParseJsonDocument(allocator, fileContents, result);

        fileContents.deinit();

        return parseResult;
    }

    struct JsonWriter
    {
        FILE *stream;
        JsonTokenType previousToken;
        collections::vector<JsonTokenType> indentTypes;
        bool shouldIndent;
        bool isBinary;

        inline JsonWriter(IAllocator allocator, FILE *fileStream, bool writerShouldIndent)
        {
            stream = fileStream;
            previousToken = JsonToken_Invalid;
            shouldIndent = writerShouldIndent;
            indentTypes = collections::vector<JsonTokenType>(allocator);
            isBinary = false;
        }
        inline void SaveAndCloseFile()
        {
            fflush(stream);
            fclose(stream);
        }
        inline void deinit()
        {
            indentTypes.deinit();
        }
        inline void WriteIndents()
        {
            if (shouldIndent)
            {
                for (usize i = 0; i < indentTypes.count; i++)
                {
                    fprintf(stream, " ");
                }
            }
        }
        inline JsonTokenType LatestIndentType()
        {
            if (indentTypes.count > 0)
            {
                return indentTypes.ptr[indentTypes.count - 1];
            }
            return JsonToken_Invalid;
        }
        inline bool WriteStartObject()
        {
            if (previousToken == JsonToken_Invalid
            || previousToken == JsonToken_LBracket
            || previousToken == JsonToken_RBrace
            || previousToken == JsonToken_BoolLiteral
            || previousToken == JsonToken_UIntegerLiteral
            || previousToken == JsonToken_IntegerLiteral
            || previousToken == JsonToken_FloatLiteral
            || previousToken == JsonToken_NullLiteral
            || previousToken == JsonToken_StringLiteral
            || previousToken == JsonToken_PropertyName)
            {
                if (previousToken != JsonToken_Invalid)
                {
                    if (previousToken == JsonToken_PropertyName)
                    {
                        fprintf(stream, ":\n");
                    }
                    else if (previousToken != JsonToken_LBracket)
                    {
                        fprintf(stream, ",\n");
                    }
                    else
                        fprintf(stream, "\n");
                }
                WriteIndents();
                fprintf(stream, "{");
                previousToken = JsonToken_LBrace;
                indentTypes.Add(JsonToken_LBrace);
                return true;
            }
            return false;
        }
        inline bool WritePropertyName(const char* chars)
        {
            if (
            previousToken == JsonToken_LBrace
            || previousToken == JsonToken_RBrace
            || previousToken == JsonToken_RBracket
            || previousToken == JsonToken_BoolLiteral 
            || previousToken == JsonToken_UIntegerLiteral
            || previousToken == JsonToken_IntegerLiteral 
            || previousToken == JsonToken_FloatLiteral 
            || previousToken == JsonToken_NullLiteral 
            || previousToken == JsonToken_StringLiteral)
            {
                if (previousToken != JsonToken_LBrace)
                {
                    fprintf(stream, ",\n");
                }
                else
                    fprintf(stream, "\n");
                WriteIndents();
                fprintf(stream, "\"%s\"", chars);
                previousToken = JsonToken_PropertyName;
                return true;
            }
            return false;
        }
        inline bool WriteIntValue(i64 value)
        {
            if (previousToken == JsonToken_PropertyName)
            {
                fprintf(stream, ": %lli", value);
                previousToken = JsonToken_IntegerLiteral;
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                if (previousToken != JsonToken_LBracket)
                {
                    fprintf(stream, ", ");
                }
                fprintf(stream, "%lli", value);
                previousToken = JsonToken_IntegerLiteral;
                return true;
            }
            return false;
        }
        inline bool WriteUintValue(u64 value)
        {
            if (previousToken == JsonToken_PropertyName)
            {
                fprintf(stream, ": %llu", value);
                previousToken = JsonToken_UIntegerLiteral;
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                if (previousToken != JsonToken_LBracket)
                {
                    fprintf(stream, ", ");
                }
                fprintf(stream, "%llu", value);
                previousToken = JsonToken_UIntegerLiteral;
                return true;
            }
            return false;
        }
        inline bool WriteFloat(double value)
        {
            if (previousToken == JsonToken_PropertyName)
            {
                fprintf(stream, ": %f", value);
                previousToken = JsonToken_FloatLiteral;
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                if (previousToken != JsonToken_LBracket)
                {
                    fprintf(stream, ", ");
                }
                fprintf(stream, "%f", value);
                previousToken = JsonToken_FloatLiteral;
                return true;
            }
            return false;
        }
        inline bool WriteBool(bool value)
        {
            if (previousToken == JsonToken_PropertyName)
            {
                fprintf(stream, ": %s", value ? "true" : "false");
                previousToken = JsonToken_BoolLiteral;
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                if (previousToken != JsonToken_LBracket)
                {
                    fprintf(stream, ", ");
                }
                fprintf(stream, "%s", value ? "true" : "false");
                previousToken = JsonToken_BoolLiteral;
                return true;
            }
            return false;
        }
        inline bool WriteString(const char* value)
        {
            if (previousToken == JsonToken_PropertyName)
            {
                fprintf(stream, ": \"%s\"", value);
                previousToken = JsonToken_StringLiteral;
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                if (previousToken != JsonToken_LBracket)
                {
                    fprintf(stream, ", ");
                }
                fprintf(stream, "\"%s\"", value);
                previousToken = JsonToken_StringLiteral;
                return true;
            }
            return false;
        }
        inline bool WriteNull()
        {
            if (previousToken == JsonToken_PropertyName)
            {
                fprintf(stream, ": null");
                previousToken = JsonToken_NullLiteral;
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                if (previousToken != JsonToken_LBracket)
                {
                    fprintf(stream, ", ");
                }
                fprintf(stream, "null");
                previousToken = JsonToken_NullLiteral;
                return true;
            }
            return false;
        }
        inline bool WriteEndObject()
        {
            if (LatestIndentType() == JsonToken_LBrace)
            {
                fprintf(stream, "\n");
                indentTypes.RemoveAt_Swap(indentTypes.count - 1);
                WriteIndents();
                fprintf(stream, "}");
                previousToken = JsonToken_RBrace;
                return true;
            }
            return false;
        }
        inline bool WriteStartArray()
        {
            if (previousToken == JsonToken_PropertyName)
            {
                fprintf(stream, ": [");
                previousToken = JsonToken_LBracket;
                indentTypes.Add(JsonToken_LBracket);
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                fprintf(stream, ", [");
                previousToken = JsonToken_LBracket;
                indentTypes.Add(JsonToken_LBracket);
                return true;
            }
            return false;
        }
        inline bool WriteEndArray()
        {
            if (LatestIndentType() == JsonToken_LBracket)
            {
                indentTypes.RemoveAt_Swap(indentTypes.count - 1);
                if (previousToken == JsonToken_RBrace)
                {
                    fprintf(stream, "\n");
                    WriteIndents();
                }
                fprintf(stream, "]");
                previousToken = JsonToken_RBracket;
                return true;
            }
            return false;
        }
    };
}

#ifdef ASTRALCORE_JSON_IMPL

namespace Json
{
    thread_local StringRentalBuffer *stringRentalBuffer = new StringRentalBuffer(GetCAllocator());
}

collections::Array<u8> Json::JsonElement::GetAsRawData(IAllocator allocator)
{
    ByteStreamWriter writer = ByteStreamWriter(GetCAllocator());
    if (this->elementType == JsonElement_Object)
    {
        for (usize i = 0; i < arrayElements.length; i++)
        {
            collections::Array<u8> toAppend = arrayElements.data[i].GetAsRawData(GetCAllocator());
            if (toAppend.data != NULL)
            {
                for (usize c = 0; c < toAppend.length; c++)
                {
                    writer.WriteByte(toAppend.data[c]);
                    //results.Add(toAppend.data[c]);
                }
                toAppend.deinit();
            }
        }
    }
    else if (this->elementType == JsonElement_Property)
    {
        Json::JsonTokenType tokenType = this->CheckElementType();
        //have to check the string to know if it's a integer, float or string
        if (tokenType == JsonToken_BoolLiteral)
        {
            writer.WriteByte((u8)data);
        }
        else if (tokenType == JsonToken_NullLiteral)
        {
            writer.WriteByte(0);
        }
        else if (tokenType == JsonToken_FloatLiteral)
        {
            writer.Write(*(float *)&data);
        }
        else if (tokenType == JsonToken_IntegerLiteral)
        {
            writer.Write(*(i32 *)&data);
        }
        else if (tokenType == JsonToken_UIntegerLiteral)
        {
            writer.Write(*(u32 *)&data);
        }
        else
        {
            writer.Write<u64>(0);
        }
    }
    return writer.bytes.ToOwnedArrayWith(allocator);
}
Json::JsonToken Json::JsonTokenizer::Next()
{
    JsonToken result;
    result.tokenType = JsonToken_Invalid;

    while (fileContents[currentIndex] == ' ' || fileContents[currentIndex] == '\n' || fileContents[currentIndex] == '\r')
    {
        if (fileContents[currentIndex] == '\n')
        {
            currentLine++;
        }
        currentIndex++;
    }
    result.startIndex = currentIndex;

    if (fileContents[currentIndex] == '\"')
    {
        currentIndex++;
        result.tokenType = JsonToken_Eof;

        while(currentIndex < length)
        {
            if (fileContents[currentIndex] == '\\')
            {
                if ((currentIndex + 1 < length) && fileContents[currentIndex + 1] == '\"' || fileContents[currentIndex + 1] == '\\')
                {
                    currentIndex += 2;
                }
                else
                    currentIndex += 1;
            }
            else
            {
                if (fileContents[currentIndex] == '\"')
                {
                    currentIndex++;
                    result.endIndex = currentIndex;
                    result.tokenType = JsonToken_StringLiteral;
                    break;
                }
                currentIndex++;
            }
        }
    }
    else if (fileContents[currentIndex] == ':')
    {
        result.tokenType = JsonToken_Colon;
        currentIndex++;
        result.endIndex = currentIndex;
    }
    else if (fileContents[currentIndex] == ',')
    {
        result.tokenType = JsonToken_Comma;
        currentIndex++;
        result.endIndex = currentIndex;
    }
    else if (fileContents[currentIndex] == '{')
    {
        result.tokenType = JsonToken_LBrace;
        currentIndex++;
        result.endIndex = currentIndex;
    }
    else if (fileContents[currentIndex] == '}')
    {
        result.tokenType = JsonToken_RBrace;
        currentIndex++;
        result.endIndex = currentIndex;
    }
    else if (fileContents[currentIndex] == '[')
    {
        result.tokenType = JsonToken_LBracket;
        currentIndex++;
        result.endIndex = currentIndex;
    }
    else if (fileContents[currentIndex] == ']')
    {
        result.tokenType = JsonToken_RBracket;
        currentIndex++;
        result.endIndex = currentIndex;
    }
    else if (fileContents[currentIndex] == '-')
    {
        currentIndex++;
        if (isdigit(fileContents[currentIndex]))
        {
            result.tokenType = JsonToken_IntegerLiteral;
            while (isdigit(fileContents[currentIndex]) || fileContents[currentIndex] == '.')
            {
                if (fileContents[currentIndex] == '.')
                {
                    //cannot do 0..0
                    if (result.tokenType == JsonToken_FloatLiteral)
                    {
                        result.tokenType = JsonToken_Invalid;
                        return result;
                    }
                    result.tokenType = JsonToken_FloatLiteral;
                }
                currentIndex += 1;
                result.endIndex = currentIndex;
            }
        }
    }
    else if (isdigit(fileContents[currentIndex]))
    {
        result.tokenType = JsonToken_UIntegerLiteral;
        while (isdigit(fileContents[currentIndex]) || fileContents[currentIndex] == '.')
        {
            if (fileContents[currentIndex] == '.')
            {
                //cannot do 0..0
                if (result.tokenType == JsonToken_FloatLiteral)
                {
                    result.tokenType = JsonToken_Invalid;
                    return result;
                }
                result.tokenType = JsonToken_FloatLiteral;
            }
            currentIndex += 1;
            result.endIndex = currentIndex;
        }
    }
    else if (currentIndex + 4 < this->length && 
    fileContents[currentIndex] == 't' && 
    fileContents[currentIndex + 1] == 'r' && 
    fileContents[currentIndex + 2] == 'u' && 
    fileContents[currentIndex + 3] == 'e')
    {
        currentIndex += 4;
        result.endIndex = currentIndex;
        result.tokenType = JsonToken_BoolLiteral;
    }
    else if (currentIndex + 4 < this->length && 
    fileContents[currentIndex] == 'n' && 
    fileContents[currentIndex + 1] == 'u' && 
    fileContents[currentIndex + 2] == 'l' && 
    fileContents[currentIndex + 3] == 'l')
    {
        currentIndex += 4;
        result.endIndex = currentIndex;
        result.tokenType = JsonToken_NullLiteral;
    }
    else if (currentIndex + 5 < this->length && 
    fileContents[currentIndex] == 'f' && 
    fileContents[currentIndex + 1] == 'a' && 
    fileContents[currentIndex + 2] == 'l' && 
    fileContents[currentIndex + 3] == 's' &&
    fileContents[currentIndex + 4] == 'e')
    {
        currentIndex += 5;
        result.endIndex = currentIndex;
        result.tokenType = JsonToken_BoolLiteral;
    }

    return result;
}
bool Json::ParseJsonElement(IAllocator allocator, JsonTokenizer *tokenizer, JsonElement *result)
{
    JsonToken peekNext = tokenizer->PeekNext();

    switch (peekNext.tokenType)
    {
        case JsonToken_StringLiteral:
        {
            *result = JsonElement(tokenizer->GetString(allocator, peekNext));
            tokenizer->Next();
            return true;
        }
        case JsonToken_IntegerLiteral:
        {
            *result = JsonElement();

            *((i64 *)&result->data) = StringToI64(tokenizer->fileContents + peekNext.startIndex, peekNext.endIndex - peekNext.startIndex);
            result->dataLength = -(i32)JsonToken_IntegerLiteral;

            tokenizer->Next();
            return true;
        }
        case JsonToken_UIntegerLiteral:
        {
            *result = JsonElement();

            result->data = StringToU64(tokenizer->fileContents + peekNext.startIndex, peekNext.endIndex - peekNext.startIndex);
            result->dataLength = -(i32)JsonToken_UIntegerLiteral;

            tokenizer->Next();
            return true;
        }
        case JsonToken_FloatLiteral:
        {
            *result = JsonElement();

            string rented = tokenizer->RentString(peekNext);
            double db = atof(rented.buffer);
            *((double *)&result->data) = db;
            Json::stringRentalBuffer->Return(rented);

            result->dataLength = -(i32)JsonToken_FloatLiteral;

            tokenizer->Next();
            return true;
        }
        case JsonToken_NullLiteral:
        {
            *result = JsonElement();
            result->dataLength = -(i32)JsonToken_NullLiteral;

            tokenizer->Next();
            return true;
        }
        case JsonToken_BoolLiteral:
        {
            *result = JsonElement();
            result->data = (u64)(strncmp(tokenizer->fileContents + peekNext.startIndex, "true", peekNext.endIndex - peekNext.startIndex) == 0);
            result->dataLength = -(i32)JsonToken_BoolLiteral;
            tokenizer->Next();
            return true;
        }
        case JsonToken_LBracket:
        {
            IAllocator cAllocator = GetCAllocator();

            collections::vector<JsonElement> arrayMembers = collections::vector<JsonElement>(cAllocator);
            tokenizer->Next();
            //empty array
            if (tokenizer->PeekNext().tokenType == JsonToken_RBracket)
            {
                tokenizer->Next();

            }
            else
            {
                while(true)
                {
                    JsonElement arrayMember;
                    if (!ParseJsonElement(allocator, tokenizer, &arrayMember))
                    {
                        //printf("Unable to parse array member\n");
                        arrayMembers.deinit();
                        return false;
                    }
                    arrayMembers.Add(arrayMember);

                    // dont actually just peek next here as if it has an error, 
                    //the resulting data won't matter even if it has been read out of order
                    peekNext = tokenizer->Next(); 
                    if (peekNext.tokenType == JsonToken_RBracket)
                    {
                        break;
                    }
                    else if (peekNext.tokenType == JsonToken_Comma)
                    {
                        continue;
                    }
                    else
                    {
                        arrayMembers.deinit();
                        return false;
                    }
                }
            }

            *result = JsonElement(arrayMembers.ToOwnedArrayWith(allocator));
            return true;
        }
        case JsonToken_LBrace:
        {
            *result = JsonElement(collections::hashmap<string, JsonElement>(allocator, &stringHash, &stringEql, 8));
            tokenizer->Next();

            collections::vector<JsonElement> childObjectsOrdered = collections::vector<JsonElement>(GetCAllocator());

            while (true)
            {
                JsonToken propertyNameToken = tokenizer->Next();
                if (propertyNameToken.tokenType == JsonToken_RBrace)
                {
                    break;
                }
                else if (propertyNameToken.tokenType != JsonToken_StringLiteral)
                {
                    return false;
                }
                string propertyName = tokenizer->GetString(allocator, propertyNameToken); // string(allocator, tokenizer->fileContents + propertyNameToken.startIndex + 1, propertyNameToken.endIndex - 1 - propertyNameToken.startIndex - 1);

                if (tokenizer->Next().tokenType != JsonToken_Colon)
                {
                    return false;
                }

                JsonElement subElementResult;
                if (!ParseJsonElement(allocator, tokenizer, &subElementResult))
                {
                    return false;
                }
                result->childObjects.Add(propertyName, subElementResult);
                childObjectsOrdered.Add(subElementResult);

                if (tokenizer->PeekNext().tokenType == JsonToken_Comma)
                {
                    tokenizer->Next();
                    continue;
                }
            }
            result->arrayElements = childObjectsOrdered.ToOwnedArrayWith(allocator);
            return true;
        }
        default:
        {
            return false;
        }
    }
}
#endif