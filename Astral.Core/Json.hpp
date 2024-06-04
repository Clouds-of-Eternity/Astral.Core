#pragma once
#include "string.hpp"
#include "allocators.hpp"
#include "ctype.h"
#include "hashmap.hpp"
#include "io.hpp"
#include "stdio.h"
#include "Maths/Util.hpp"

namespace Json
{
    enum JsonTokenType
    {
        JsonToken_Invalid,
        JsonToken_StringLiteral,
        JsonToken_IntegerLiteral,
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
        //union
        //{
            collections::hashmap<string, JsonElement> childObjects;
            collections::Array<JsonElement> arrayElements;
            string value;
        //};
        JsonElementType elementType;

        inline JsonElement()
        {
            this->value = string();
            elementType = JsonElement_Property;
        }
        inline JsonElement(string stringValue)
        {
            this->value = stringValue;
            elementType = JsonElement_Property;
        }
        inline JsonElement(collections::hashmap<string, JsonElement> thisChildObjects)
        {
            this->childObjects = thisChildObjects;
            elementType = JsonElement_Object;
        }
        inline JsonElement(collections::Array<JsonElement> childElements)
        {
            this->arrayElements = childElements;
            elementType = JsonElement_Array;
        }
        
        inline JsonTokenType CheckElementType()
        {
            JsonTokenType tokenType = JsonToken_IntegerLiteral;

            if (this->elementType == JsonElement_Array)
            {
                return JsonToken_LBracket;
            }
            if (this->elementType == JsonElement_Object)
            {
                return JsonToken_LBrace;
            }
            //this is not possible unless we are a string "" so
            if (this->value.length == 0)
            {
                return JsonToken_StringLiteral;
            }
            else
            {
                for (usize i = 0; i < this->value.length - 1; i++)
                {
                    if ((this->value.buffer[i] == '-' && i > 0) || !isdigit(this->value.buffer[i]))
                    {
                        if (this->value.buffer[i] == '.')
                        {
                            if (tokenType == JsonToken_FloatLiteral)
                            {
                                tokenType = JsonToken_StringLiteral;
                            }
                            else
                            {
                                tokenType = JsonToken_FloatLiteral;
                            }
                        }
                        else
                        {
                            tokenType = JsonToken_StringLiteral;
                            break;
                        }
                    }
                }
            }
            if (tokenType == JsonToken_StringLiteral)
            {
                if (value == "true" || value == "false")
                {
                    tokenType = JsonToken_BoolLiteral;
                }
                else if (value == "null")
                {
                    tokenType = JsonToken_NullLiteral;
                }
            }
            return tokenType;
        }
        inline void GetInteger(void* output, i8* type)
        {
            if (elementType != JsonElement_Property)
            {
                return;
            }
            u64 result = 0;
            u64 index = 1;
            for (i64 i = value.length - 1; i >= 0; i--)
            {
                if (value.buffer[i] >= '0' && value.buffer[i] <= '9')
                {
                    u64 amount = index * (value.buffer[i] - (u64)'0');
                    result += amount;
                    index *= 10;
                }
            }
            if (value.buffer[0] == '-')
            {
                if (result < I8Max)
                {
                    i8 i8result = (i8)result * -1;
                    *((i8 *)output) = i8result;
                    *type = -1;
                }
                else if (result < I16Max)
                {
                    i16 i16result = (i16)result * -1;
                    *((i16 *)output) = i16result;
                    *type = -2;
                }
                else if (result < I32Max)
                {
                    i32 i32result = (i32)result * -1;
                    *((i32 *)output) = i32result;
                    *type = -3;
                }
                else// if (result > I64Max)
                {
                    if (result > I64Max)
                    {
                        result = I64Max;
                    }
                    i64 i64result = (i64)result * -1;
                    *((i64 *)output) = i64result;
                    *type = -4;
                }
            }
            else
            {
                if (result < U8Max)
                {
                    u8 u8result = (u8)result;
                    *((u8 *)output) = u8result;
                    *type = 1;
                }
                else if (result < U16Max)
                {
                    u16 u16result = (u16)result;
                    *((u16 *)output) = u16result;
                    *type = 2;
                }
                else if (result < U32Max)
                {
                    u32 u32result = (u32)result;
                    *((u32 *)output) = u32result;
                    *type = 3;
                }
                else
                {
                    u64 u64result = (u64)result;
                    *((u64 *)output) = u64result;
                    *type = 4;
                }
            }
        }
        inline i32 GetInt32()
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }

            i32 result = 0;
            i32 index = 1;
            for (i64 i = value.length - 1; i >= 0; i--)
            {
                if (value.buffer[i] >= '0' && value.buffer[i] <= '9')
                {
                    i32 amount = index * (value.buffer[i] - (i32)'0');
                    result += amount;
                    index *= 10;
                }
                else if (value.buffer[i] == '-' && i == 0)
                {
                    result *= -1;
                }
            }

            return result;
        }
        inline u32 GetUint32()
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }

            u32 result = 0;
            u32 index = 1;
            for (i64 i = value.length - 1; i >= 0; i--)
            {
                if (value.buffer[i] >= '0' && value.buffer[i] <= '9')
                {
                    u32 amount = index * (value.buffer[i] - (u32)'0');
                    result += amount;
                    index *= 10;
                }
            }

            return result;
        }
        inline bool GetBool()
        {
            if (elementType != JsonElement_Property)
            {
                return false;
            }

            if (value == "true")
            {
                return true;
            }
            return false;
        }
        inline float GetFloat()
        {
            if (elementType != JsonElement_Property)
            {
                return 0.0f;
            }
            double result = atof(value.buffer);

            return (float)result;
        }
        inline double GetDouble()
        {
            if (elementType != JsonElement_Property)
            {
                return 0.0;
            }
            double result = atof(value.buffer);

            return result;
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
            collections::vector<CharSlice> charSlices = collections::vector<CharSlice>(GetCAllocator());
            usize start = 0;
            for (usize i = 0; i < value.length - 1; i++)
            {
                if (value.buffer[i] == '\\' && i < value.length - 2)
                {
                    if (value.buffer[i + 1] == '\"')
                    {
                        charSlices.Add(CharSlice(value.buffer + start, i - start));
                        charSlices.Add(CharSlice("\""));
                        i++;
                        start = i + 1;
                    }
                    else if (value.buffer[i + 1] == 'n')
                    {
                        charSlices.Add(CharSlice(value.buffer + start, i - start));
                        charSlices.Add(CharSlice("\n"));
                        i++;
                        start = i + 1;
                    }
                }
            }
            if (start < value.length - 1)
            {
                charSlices.Add(CharSlice(value.buffer + start, value.length - 1 - start));
            }
            string result = ConcatFromCharSlices(allocator, charSlices.ptr, charSlices.count);
            charSlices.deinit();
            return result;
        }
        inline string GetStringRaw(IAllocator allocator)
        {
            return string(allocator, value.buffer);
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
                
                string propertyName = string(allocator, ptr);
                JsonElement *result = childObjects.Get(propertyName);
                propertyName.deinit();
                return result;
            }
            return NULL;
        }
        /// @brief Parses all child objects recursively, collecting their properties 
        /// and aggregating them into raw byte data
        /// @return the Json object as raw data
        collections::Array<u8> GetAsRawData(IAllocator allocator);
        void DumpJsonToStdout(i32 indents);
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

        inline JsonWriter(IAllocator allocator, FILE *fileStream, bool writerShouldIndent)
        {
            stream = fileStream;
            previousToken = JsonToken_Invalid;
            shouldIndent = writerShouldIndent;
            indentTypes = collections::vector<JsonTokenType>(allocator);
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
            previousToken == JsonToken_LBrace || 
            previousToken == JsonToken_RBrace ||
            previousToken == JsonToken_RBracket ||
            previousToken == JsonToken_BoolLiteral || 
            previousToken == JsonToken_IntegerLiteral || 
            previousToken == JsonToken_FloatLiteral || 
            previousToken == JsonToken_NullLiteral || 
            previousToken == JsonToken_StringLiteral)
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
                previousToken = JsonToken_IntegerLiteral;
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                if (previousToken != JsonToken_LBracket)
                {
                    fprintf(stream, ", ");
                }
                fprintf(stream, "%llu", value);
                previousToken = JsonToken_IntegerLiteral;
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
collections::Array<u8> Json::JsonElement::GetAsRawData(IAllocator allocator)
{
    collections::vector<u8> results = collections::vector<u8>(GetCAllocator());
    if (this->elementType == JsonElement_Object)
    {
        for (usize i = 0; i < arrayElements.length; i++)
        {
            collections::Array<u8> toAppend = arrayElements.data[i].GetAsRawData(GetCAllocator());
            if (toAppend.data != NULL)
            {
                for (usize c = 0; c < toAppend.length; c++)
                {
                    results.Add(toAppend.data[c]);
                }
                toAppend.deinit();
            }
        }
    }
    else if (this->elementType == JsonElement_Property)
    {
        //have to check the string to know if it's a integer, float or string
        if (this->value == "true")
        {
            results.Add(1);
        }
        else if (this->value == "false" || this->value == "null")
        {
            results.Add(0);
        }
        else
        {
            // reuse tokenType;
            Json::JsonTokenType tokenType = this->CheckElementType();

            if (tokenType == JsonToken_BoolLiteral)
            {
                results.Add(this->value == "true" ? 1 : 0);
            }
            if (tokenType == JsonToken_FloatLiteral)
            {
                float parsed = atof(this->value.buffer);
                for (i32 i = 0; i < 4; i++)
                {
                    results.Add(((u8*)&parsed)[i]);
                }
            }
            else if (tokenType == JsonToken_IntegerLiteral)
            {
                i32 integer = atoi(this->value.buffer);
                for (i32 i = 0; i < 4; i++)
                {
                    results.Add(((u8*)&integer)[i]);
                }
            }
            else
            {
                for (i32 i = 0; i < sizeof(char *); i++)
                {
                    results.Add(0);
                }
            }
        }
    }
    return results.ToOwnedArrayWith(allocator);
}
void Json::JsonElement::DumpJsonToStdout(i32 indents)
{
    if (this->elementType == JsonElement_Property)
    {
        printf("%s", this->value.buffer);
    }
    else if (this->elementType == JsonElement_Array)
    {
        printf("[");
        if (this->arrayElements.length > 0)
        {
            printf("\n");
        }
        for (usize i = 0; i < this->arrayElements.length; i++)
        {
            for (usize c = 0; c < indents + 1; c++)
            {
                printf(" ");
            }
            this->arrayElements.data[i].DumpJsonToStdout(indents + 1);
            if (i < (i32)this->arrayElements.length - 1)
            {
                printf(",\n");
            }
            else
                printf("\n");
        }
        for (usize c = 0; c < indents; c++)
        {
            printf(" ");
        }
        printf("]");
    }
    else
    {
        printf("{\n");
        usize iterated = 0;
        for (usize i = 0; i < this->childObjects.bucketsCount; i++)
        {
            if (this->childObjects.buckets[i].initialized)
            {
                for (usize j = 0; j < this->childObjects.buckets[i].entries.count; j++)
                {
                    for (usize c = 0; c < indents + 1; c++)
                    {
                        printf(" ");
                    }
                        printf("%s: ", this->childObjects.buckets[i].entries.ptr[j].key.buffer);
                    this->childObjects.buckets[i].entries.ptr[j].value.DumpJsonToStdout(indents + 1);
                    if (iterated < this->childObjects.Count - 1)
                    {
                        printf(",\n");
                    }
                    else printf("\n");
                    iterated++;
                }
            }
        }
        for (usize c = 0; c < indents; c++)
        {
            printf(" ");
        }
        printf("}");
    }
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
    if (peekNext.tokenType == JsonToken_StringLiteral || peekNext.tokenType == JsonToken_IntegerLiteral || peekNext.tokenType == JsonToken_FloatLiteral || peekNext.tokenType == JsonToken_BoolLiteral || peekNext.tokenType == JsonToken_NullLiteral)
    {
        *result = JsonElement(tokenizer->GetString(allocator, peekNext));
        tokenizer->Next();
        return true;
    }
    else if (peekNext.tokenType == JsonToken_LBracket) //[]
    {
        //printf("Entering array\n");
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
    else if (peekNext.tokenType == JsonToken_LBrace) //{}
    {
        *result = JsonElement(collections::hashmap<string, JsonElement>(allocator, &stringHash, &stringEql));
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
    return false;
}
#endif