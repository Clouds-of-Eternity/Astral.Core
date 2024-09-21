#pragma once
#include "Json.hpp"
#include "binaryio.hpp"
#include "ByteStreamOps.hpp"

namespace Json
{
    void JsonConvertToBinary(FILE *fs, JsonElement *json);
    void ParseJsonBfmtElement(IAllocator allocator, ByteStreamReader *reader, JsonElement *result);

    enum JsonBinaryFieldType
    {
        JsonBinaryField_Property,
        JsonBinaryField_Array,
        JsonBinaryField_String,
        JsonBinaryField_Float,
        JsonBinaryField_Bool,
        JsonBinaryField_Null,
        JsonBinaryField_Int8,
        JsonBinaryField_Int16,
        JsonBinaryField_Int32,
        JsonBinaryField_Int64
    };
}

#ifdef ASTRALCORE_JSON_IMPL

void Json::ParseJsonBfmtElement(IAllocator allocator, ByteStreamReader *reader, JsonElement *result)
{
    JsonBinaryFieldType fieldType = (JsonBinaryFieldType)reader->Read<u8>();
    if (fieldType == JsonBinaryField_Property)
    {
        u32 count = reader->Read<u32>();

        *result = JsonElement(collections::hashmap<string, JsonElement>(allocator, &stringHash, &stringEql));
        result->arrayElements = collections::Array<JsonElement>(allocator, count);
        for (u32 i = 0; i < count; i++)
        {
            string key = reader->ReadString(allocator);
            JsonElement value;
            ParseJsonBfmtElement(allocator, reader, &value);
            result->childObjects.Add(key, value);
            result->arrayElements.data[i] = value;
        }
    }
    else if (fieldType == JsonBinaryField_Array)
    {
        u32 count = reader->Read<u32>();

        *result = JsonElement(collections::Array<JsonElement>(allocator, count));
        for (u32 i = 0; i < count; i++)
        {
            JsonElement value;
            ParseJsonBfmtElement(allocator, reader, &value);
            result->arrayElements.data[i] = value;
        }
    }
    else
    {
        result->value = string();
        result->value.allocator = allocator;
        switch (fieldType)
        {
            case JsonBinaryField_Null:
            {
                result->value = string();
                break;
            }
            case JsonBinaryField_String:
            {
                result->value = reader->ReadString(allocator);
                break;
            }
            case JsonBinaryField_Bool:
            {
                result->value.length = 1;
                result->value.buffer = (char *)malloc(1);
                result->value.buffer[0] = reader->Read<bool>() ? 1 : 0;
                break;
            }
            case JsonBinaryField_Float:
            {
                result->value.length = 4;
                result->value.buffer = (char *)malloc(4);
                *(float *)result->value.buffer = reader->Read<float>();
                break;
            }
            case JsonBinaryField_Int8:
            {
                result->value.length = 1;
                result->value.buffer = (char *)malloc(1);
                result->value.buffer[0] = reader->Read<u8>();
                break;
            }
            case JsonBinaryField_Int16:
            {
                result->value.length = 2;
                result->value.buffer = (char *)malloc(2);
                *(u16 *)result->value.buffer = reader->Read<u16>();
                break;
            }
            case JsonBinaryField_Int32:
            {
                result->value.length = 4;
                result->value.buffer = (char *)malloc(4);
                *(u32 *)result->value.buffer = reader->Read<u32>();
                break;
            }
            case JsonBinaryField_Int64:
            {
                result->value.length = 8;
                result->value.buffer = (char *)malloc(8);
                *(u64 *)result->value.buffer = reader->Read<u64>();
                break;
            }
            default:
                break;
        }
    }
}

void Json::JsonConvertToBinary(FILE *fs, JsonElement *json)
{
    switch (json->elementType)
    {
        case JsonElement_Property:
        {
            Binary_WriteData<u8>(fs, (u8)JsonBinaryField_Property);
            Binary_WriteData<u32>(fs, (u32)json->childObjects.count);
            
            auto iterator = json->childObjects.GetIterator();
            foreach (kvp, iterator)
            {
                Binary_WriteString(fs, kvp->key);
                JsonConvertToBinary(fs, &kvp->value);
            }
            break;
        }
        case JsonElement_Array:
        {
            Binary_WriteData<u8>(fs, (u8)JsonBinaryField_Array);
            Binary_WriteData<u32>(fs, (u32)json->arrayElements.length);
            for (usize i = 0; i < json->arrayElements.length; i++)
            {
                JsonConvertToBinary(fs, &json->arrayElements.data[i]);
            }
            break;
        }
        case JsonElement_Object:
        {
            Json::JsonTokenType elemType = json->CheckElementType();
            switch (elemType)
            {
                case JsonToken_StringLiteral:
                {
                    Binary_WriteData<u8>(fs, (u8)JsonBinaryField_String);
                    Binary_WriteString(fs, json->value);
                    break;
                }
                case JsonToken_FloatLiteral:
                {
                    Binary_WriteData<u8>(fs, (u8)JsonBinaryField_Float);
                    Binary_WriteData<float>(fs, json->GetFloat());
                    break;
                }
                case JsonToken_BoolLiteral:
                {
                    Binary_WriteData<u8>(fs, (u8)JsonBinaryField_Bool);
                    Binary_WriteData<bool>(fs, json->GetBool());
                    break;
                }
                case JsonToken_NullLiteral:
                {
                    Binary_WriteData<u8>(fs, (u8)JsonBinaryField_Null);
                    Binary_WriteText(fs, "null");
                    break;
                }
                case JsonToken_IntegerLiteral:
                {
                    i64 output = 0;
                    Json::JsonIntegerType type;
                    json->GetInteger(&output, &type);
                    if (type == Json::JsonInteger_I8 || type == Json::JsonInteger_U8)
                    {
                        i8 data = (i8)output;
                        Binary_WriteData<u8>(fs, (u8)JsonBinaryField_Int8);
                        Binary_WriteData<i8>(fs, data);
                    }
                    else if (type == Json::JsonInteger_I16 || type == Json::JsonInteger_U16)
                    {
                        i16 data = (i16)output;
                        Binary_WriteData<u8>(fs, (u8)JsonBinaryField_Int16);
                        Binary_WriteData<i16>(fs, data);
                    }
                    else if (type == Json::JsonInteger_I32 || type == Json::JsonInteger_U32)
                    {
                        i32 data = (i32)output;
                        Binary_WriteData<u8>(fs, (u8)JsonBinaryField_Int32);
                        Binary_WriteData<i32>(fs, data);
                    }
                    else
                    {
                        Binary_WriteData<u8>(fs, (u8)JsonBinaryField_Int64);
                        Binary_WriteData<i64>(fs, output);
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
    }
}

#endif