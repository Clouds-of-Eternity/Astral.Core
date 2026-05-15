#pragma once

#include "string.hpp"
#include "Allocators.hpp"
#include "ctype.h"
#include "stdio.h"
#include "ByteStreamOps.hpp"
#include "NumericTypeChecking.hpp"

namespace Json
{
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
            this->currentLine = 1;
        }
        inline string GetString(IAllocator allocator, JsonToken token)
        {
            if (token.tokenType == JsonToken_StringLiteral)
            {
                return string(allocator, fileContents + token.startIndex + 1, token.endIndex - token.startIndex - 2);
            }
            return string(allocator, fileContents + token.startIndex, token.endIndex - token.startIndex);
        }
        inline u32 GetAsString(JsonToken token, char* output, u32 maxLength)
        {
            u32 copyLength;
            if (token.tokenType == JsonToken_StringLiteral)
            {
                copyLength = token.endIndex - token.startIndex - 2;
                memcpy(output, &fileContents[token.startIndex + 1], copyLength < maxLength ? copyLength : maxLength);
            }
            else
            {
                copyLength = token.endIndex - token.startIndex;
                memcpy(output, &fileContents[token.startIndex], copyLength < maxLength ? copyLength : maxLength);
            }
            return copyLength;
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
    struct JsonProperty;
    struct JsonElement
    {
        collections::Array<JsonProperty> arrayElements;
        u64 data;
        //negative numbers = typed data
        //positive numbers = length of string
        i32 dataLength;

        JsonElementType elementType;

        inline JsonElement()
        {
            data = 0;
            dataLength = 0;
            arrayElements = collections::Array<JsonProperty>();
            elementType = JsonElement_Property;
        }
        inline JsonElement(string stringValue)
        {
            data = (u64)stringValue.buffer;
            dataLength = (i32)stringValue.length;
            elementType = JsonElement_Property;
            arrayElements = collections::Array<JsonProperty>();
        }
        
        inline JsonTokenType CheckValueType() const
        {
            if (dataLength < 0)
            {
                return (JsonTokenType)(-dataLength);
            }
            return JsonToken_StringLiteral;
        }
        inline i8 GetInt8() const
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(i8 *)&data;
        }
        inline u8 GetUint8() const
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(u8 *)&data;
        }
        inline i16 GetInt16() const
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(i16 *)&data;
        }
        inline u16 GetUint16() const
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(u16 *)&data;
        }
        inline i32 GetInt32() const
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(i32 *)&data;
        }
        inline u32 GetUint32() const
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(u32 *)&data;
        }
        inline i64 GetInt64() const
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return *(i64 *)&data;
        }
        inline u64 GetUint64() const
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }
            return data;
        }
        inline bool GetBool() const
        {
            if (elementType != JsonElement_Property)
            {
                return false;
            }
            return (bool)data;
        }
        inline float GetFloat() const
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
        inline double GetDouble() const
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
        inline string GetString(IAllocator allocator) const
        {
            if (dataLength < 0)
            {
                return string();
            }
            collections::List<CharSlice> charSlices = collections::List<CharSlice>(GetCAllocator());
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
        inline string GetStringRaw(IAllocator allocator) const
        {
            if (dataLength < 0)
            {
                return string();
            }
            return string(allocator, (char *)data);
        }
        inline string AsString() const
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
            struct JsonPropertyImpl
            {
                string name;
                JsonElement value;
            };
            T result = T(); //invoke default constructor
            u8 *asPtr = (u8*)&result;
            JsonPropertyImpl *asImpl = (JsonPropertyImpl *)this->arrayElements.data;
            for (usize i = 0; i < this->arrayElements.length; i++)
            {
                u8 byte = (u8)asImpl[i].value.GetUint32();
                asPtr[i] = byte;
            }
            return result;
        }
        JsonElement *GetProperty(string propertyName) const;
        JsonElement *GetProperty(text propertyName) const;

        /// @brief Parses all child objects recursively, collecting their properties 
        /// and aggregating them into raw byte data
        /// @return the Json object as raw data
        collections::Array<u8> GetAsRawData(IAllocator allocator);
    };
    struct JsonProperty
    {
        string key;
        JsonElement value;
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

    struct JsonWriter
    {
        IDataStream stream;
        JsonTokenType previousToken;
        collections::List<JsonTokenType> indentTypes;
        u8 indentsCount;

        inline JsonWriter(IAllocator allocator, FILE *fileStream, u8 indentsCount)
        {
            stream = GetWriteFileDataStream(fileStream, true);
            previousToken = JsonToken_Invalid;
            this->indentsCount = indentsCount;
            indentTypes = collections::List<JsonTokenType>(allocator);
        }
        inline JsonWriter(IAllocator allocator, IDataStream dataStream, u8 indentsCount)
        {
            stream = dataStream;
            previousToken = JsonToken_Invalid;
            this->indentsCount = indentsCount;
            indentTypes = collections::List<JsonTokenType>(allocator);
        }
        inline void SaveAndCloseFile()
        {
            stream.Flush();
            stream.deinit();
        }
        inline void deinit()
        {
            indentTypes.deinit();
        }
        inline void WriteIndents()
        {
            if (indentsCount > 0)
            {
                for (usize i = 0; i < indentTypes.count * indentsCount; i++)
                {
                    stream.WriteByte(' ');
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
                        stream.WriteText(":\n");
                    }
                    else if (previousToken != JsonToken_LBracket)
                    {
                        stream.WriteText(",\n");
                    }
                    else
                        stream.WriteText("\n");
                }
                WriteIndents();
                stream.WriteText("{");
                previousToken = JsonToken_LBrace;
                indentTypes.Add(JsonToken_LBrace);
                return true;
            }
            return false;
        }
        inline bool WritePropertyNameCharSlice(CharSlice slice)
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
                    stream.WriteText(",\n");
                }
                else
                {
                    stream.WriteText("\n");
                }
                WriteIndents();
                stream.WriteByte('\"');
                stream.WriteCharSlice(slice);
                stream.WriteByte('\"');
                //stream.WriteFormatted("\"%s\"", chars);
                previousToken = JsonToken_PropertyName;
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
                    stream.WriteText(",\n");
                }
                else
                {
                    stream.WriteText("\n");
                }
                WriteIndents();
                stream.WriteFormatted("\"%s\"", chars);
                previousToken = JsonToken_PropertyName;
                return true;
            }
            return false;
        }
        inline bool WriteIntValue(i64 value)
        {
            if (previousToken == JsonToken_PropertyName)
            {
                stream.WriteFormatted(": %lli", value);
                previousToken = JsonToken_IntegerLiteral;
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                if (previousToken != JsonToken_LBracket)
                {
                    stream.WriteText(", ");
                }
                stream.WriteFormatted("%lli", value);
                previousToken = JsonToken_IntegerLiteral;
                return true;
            }
            return false;
        }
        inline bool WriteUintValue(u64 value)
        {
            if (previousToken == JsonToken_PropertyName)
            {
                stream.WriteFormatted(": %llu", value);
                previousToken = JsonToken_UIntegerLiteral;
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                if (previousToken != JsonToken_LBracket)
                {
                    stream.WriteText(", ");
                }
                stream.WriteFormatted("%llu", value);
                previousToken = JsonToken_UIntegerLiteral;
                return true;
            }
            return false;
        }
        inline bool WriteFloat(double value)
        {
            if (previousToken == JsonToken_PropertyName)
            {
                stream.WriteFormatted(": %f", value);
                previousToken = JsonToken_FloatLiteral;
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                if (previousToken != JsonToken_LBracket)
                {
                    stream.WriteText(", ");
                }
                stream.WriteFormatted("%f", value);
                previousToken = JsonToken_FloatLiteral;
                return true;
            }
            return false;
        }
        inline bool WriteBool(bool value)
        {
            if (previousToken == JsonToken_PropertyName)
            {
                stream.WriteFormatted(": %s", value ? "true" : "false");
                previousToken = JsonToken_BoolLiteral;
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                if (previousToken != JsonToken_LBracket)
                {
                    stream.WriteText(", ");
                }
                stream.WriteFormatted("%s", value ? "true" : "false");
                previousToken = JsonToken_BoolLiteral;
                return true;
            }
            return false;
        }
        inline bool WriteString(const char* value)
        {
            if (previousToken == JsonToken_PropertyName)
            {
                stream.WriteFormatted(": \"%s\"", value);
                previousToken = JsonToken_StringLiteral;
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                if (previousToken != JsonToken_LBracket)
                {
                    stream.WriteText(", ");
                }
                stream.WriteFormatted("\"%s\"", value);
                previousToken = JsonToken_StringLiteral;
                return true;
            }
            return false;
        }
        inline bool WriteNull()
        {
            if (previousToken == JsonToken_PropertyName)
            {
                stream.WriteText(": null");
                previousToken = JsonToken_NullLiteral;
                return true;
            }
            else if (LatestIndentType() == JsonToken_LBracket)
            {
                if (previousToken != JsonToken_LBracket)
                {
                    stream.WriteText(", ");
                }
                stream.WriteText("null");
                previousToken = JsonToken_NullLiteral;
                return true;
            }
            return false;
        }
        inline bool WriteEndObject()
        {
            if (LatestIndentType() == JsonToken_LBrace)
            {
                stream.WriteText("\n");
                indentTypes.RemoveAt_Swap(indentTypes.count - 1);
                WriteIndents();
                stream.WriteText("}");
                previousToken = JsonToken_RBrace;
                return true;
            }
            return false;
        }
        inline bool WriteStartArray()
        {
            if (previousToken == JsonToken_PropertyName)
            {
                stream.WriteText(": [");
                previousToken = JsonToken_LBracket;
                indentTypes.Add(JsonToken_LBracket);
                return true;
            }
            else if (previousToken != JsonToken_LBracket)
            {
                stream.WriteText(", ");
            }

            stream.WriteText("[");
            previousToken = JsonToken_LBracket;
            indentTypes.Add(JsonToken_LBracket);
            return true;
        }
        inline bool WriteEndArray()
        {
            if (LatestIndentType() == JsonToken_LBracket)
            {
                indentTypes.RemoveAt_Swap(indentTypes.count - 1);
                if (previousToken == JsonToken_RBrace)
                {
                    stream.WriteText("\n");
                    WriteIndents();
                }
                stream.WriteText("]");
                previousToken = JsonToken_RBracket;
                return true;
            }
            return false;
        }
    };
}

#ifdef ASTRALCORE_JSON_IMPL

Json::JsonElement *Json::JsonElement::GetProperty(text propertyName) const
{
    if (this->elementType == JsonElement_Object)
    {
        for (u32 i = 0; i < arrayElements.length; i++)
        {
            if (arrayElements.data[i].key == propertyName)
            {
                return &arrayElements.data[i].value;
            }
        }
        //return childObjects.Get(propertyName);
    }
    return NULL;
}
Json::JsonElement *Json::JsonElement::GetProperty(string propertyName) const
{
    if (this->elementType == JsonElement_Object)
    {
        for (u32 i = 0; i < arrayElements.length; i++)
        {
            if (arrayElements.data[i].key == propertyName)
            {
                return &arrayElements.data[i].value;
            }
        }
        //return childObjects.Get(propertyName);
    }
    return NULL;
}
collections::Array<u8> Json::JsonElement::GetAsRawData(IAllocator allocator)
{
    ByteStreamWriter writer = ByteStreamWriter(GetCAllocator());
    if (this->elementType == JsonElement_Object)
    {
        for (usize i = 0; i < arrayElements.length; i++)
        {
            collections::Array<u8> toAppend = arrayElements.data[i].value.GetAsRawData(GetCAllocator());
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
        Json::JsonTokenType tokenType = this->CheckValueType();
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

    while (fileContents[currentIndex] == ' ' || fileContents[currentIndex] == '\t' || fileContents[currentIndex] == '\n' || fileContents[currentIndex] == '\r')
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
    else if (CharIsPossibleNumericMember(fileContents[currentIndex]))
    {
        usize start = currentIndex;
        do
        {
            currentIndex++;
        }
        while (CharIsPossibleNumericMember(fileContents[currentIndex]));

        CharSlice numericStr = CharSlice(fileContents + start, currentIndex - start);
        NumericType numericType = CheckStringNumericType(numericStr);

        result.endIndex = currentIndex;
        if (numericType == NumericType_None)
        {
            result.tokenType = Json::JsonToken_Invalid;
        }
        else if (numericType == NumericType_UInteger)
        {
            result.tokenType = Json::JsonToken_UIntegerLiteral;
        }
        else if (numericType == NumericType_Integer)
        {
            result.tokenType = Json::JsonToken_IntegerLiteral;
        }
        else
        {
            result.tokenType = Json::JsonToken_FloatLiteral;
        }
    }

    return result;
}
bool Json::ParseJsonElement(IAllocator allocator, JsonTokenizer *tokenizer, JsonElement *result)
{
    JsonToken peekNext = tokenizer->Next();

    switch (peekNext.tokenType)
    {
        case JsonToken_StringLiteral:
        {
            *result = JsonElement(tokenizer->GetString(allocator, peekNext));
            return true;
        }
        case JsonToken_IntegerLiteral:
        {
            *result = JsonElement();

            *((i64 *)&result->data) = TextToI64(tokenizer->fileContents + peekNext.startIndex, peekNext.endIndex - peekNext.startIndex);
            result->dataLength = -(i32)JsonToken_IntegerLiteral;

            return true;
        }
        case JsonToken_UIntegerLiteral:
        {
            *result = JsonElement();

            result->data = TextToU64(tokenizer->fileContents + peekNext.startIndex, peekNext.endIndex - peekNext.startIndex);
            result->dataLength = -(i32)JsonToken_UIntegerLiteral;

            return true;
        }
        case JsonToken_FloatLiteral:
        {
            *result = JsonElement();

            char floatLiteralStr[32];
            u32 copied = tokenizer->GetAsString(peekNext, floatLiteralStr, 31);
            floatLiteralStr[copied] = '\0';
            double db = atof(floatLiteralStr);
            *((double *)&result->data) = db;

            result->dataLength = -(i32)JsonToken_FloatLiteral;

            return true;
        }
        case JsonToken_NullLiteral:
        {
            *result = JsonElement();
            result->dataLength = -(i32)JsonToken_NullLiteral;

            return true;
        }
        case JsonToken_BoolLiteral:
        {
            *result = JsonElement();
            result->data = (u64)(strncmp(tokenizer->fileContents + peekNext.startIndex, "true", peekNext.endIndex - peekNext.startIndex) == 0);
            result->dataLength = -(i32)JsonToken_BoolLiteral;

            return true;
        }
        case JsonToken_LBracket:
        {
            IAllocator cAllocator = GetCAllocator();

            collections::List<JsonProperty> arrayMembers = collections::List<JsonProperty>(cAllocator);

            //empty array
            if (tokenizer->PeekNext().tokenType == JsonToken_RBracket)
            {
                tokenizer->Next();
            }
            else
            {
                while(true)
                {
                    JsonProperty arrayMember;
                    arrayMember.key = string();
                    if (!ParseJsonElement(allocator, tokenizer, &arrayMember.value))
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

            result->arrayElements = arrayMembers.ToOwnedArrayWith(allocator);
            result->elementType = JsonElement_Array;
            result->data = 0;
            result->dataLength = -(i32)JsonToken_LBracket;
            return true;
        }
        case JsonToken_LBrace:
        {
            *result = JsonElement();

            collections::List<JsonProperty> childObjectsOrdered = collections::List<JsonProperty>(GetCAllocator());

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

                if (tokenizer->Next().tokenType != JsonToken_Colon)
                {
                    return false;
                }

                JsonProperty subElementResult;
                if (!ParseJsonElement(allocator, tokenizer, &subElementResult.value))
                {
                    return false;
                }
                subElementResult.key = tokenizer->GetString(allocator, propertyNameToken);
                childObjectsOrdered.Add(subElementResult);
                
                if (tokenizer->PeekNext().tokenType == JsonToken_Comma)
                {
                    tokenizer->Next();
                    continue;
                }
            }
            result->arrayElements = childObjectsOrdered.ToOwnedArrayWith(allocator);
            result->elementType = JsonElement_Object;
            result->data = 0;
            result->dataLength = -(i32)JsonToken_LBrace;
            return true;
        }
        default:
        {
            return false;
        }
    }
}
#endif