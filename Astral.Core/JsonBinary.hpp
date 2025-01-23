#pragma once
#include "Json.hpp"
#include "binaryio.hpp"
#include "ByteStreamOps.hpp"

namespace Json
{
    void JsonConvertToBinary(ByteStreamWriter *writer, JsonElement *json);
    void ParseJsonBfmtElement(IAllocator allocator, ByteStreamReader *reader, JsonElement *result);

    enum JsonBinaryFieldType
    {
        JsonBinaryField_Object,
        JsonBinaryField_Array,
        JsonBinaryField_String,
        JsonBinaryField_Float,
        JsonBinaryField_Bool,
        JsonBinaryField_Null,
        JsonBinaryField_Int8,
        JsonBinaryField_Int16,
        JsonBinaryField_Int32,
        JsonBinaryField_Int64,
        JsonBinaryField_UInt8,
        JsonBinaryField_UInt16,
        JsonBinaryField_UInt32,
        JsonBinaryField_UInt64
    };
}

#ifdef ASTRALCORE_JSON_IMPL

void Json::ParseJsonBfmtElement(IAllocator allocator, ByteStreamReader *reader, JsonElement *result)
{
    JsonBinaryFieldType fieldType = (JsonBinaryFieldType)reader->Read<u8>();
    if (fieldType == JsonBinaryField_Object)
    {
        result->elementType = JsonElement_Object;
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
        result->elementType = JsonElement_Array;
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
        result->elementType = JsonElement_Property;
        result->dataLength = 0;
        result->data = 0;
        switch (fieldType)
        {
            case JsonBinaryField_Null:
            {
                result->dataLength = -(i32)JsonToken_NullLiteral;
                //result->value = string();
                break;
            }
            case JsonBinaryField_String:
            {
                string str = reader->ReadString(allocator);
                result->data = (u64)str.buffer;
                result->dataLength = str.length;
                break;
            }
            case JsonBinaryField_Bool:
            {
                result->data = (u64)reader->Read<bool>();
                result->dataLength = -(i32)JsonToken_BoolLiteral;
                break;
            }
            case JsonBinaryField_Float:
            {
                *((float *)&result->data) = reader->Read<float>();
                result->dataLength = -(i32)JsonToken_FloatLiteral;
                break;
            }
            case JsonBinaryField_Int8:
            {
                *((i8 *)&result->data) = reader->Read<i8>();
                result->dataLength = -(i32)JsonToken_IntegerLiteral;
                break;
            }
            case JsonBinaryField_Int16:
            {
                *((i16 *)&result->data) = reader->Read<i16>();
                result->dataLength = -(i32)JsonToken_IntegerLiteral;
                break;
            }
            case JsonBinaryField_Int32:
            {
                *((i32 *)&result->data) = reader->Read<i32>();
                result->dataLength = -(i32)JsonToken_IntegerLiteral;
                break;
            }
            case JsonBinaryField_Int64:
            {
                *((i64 *)&result->data) = reader->Read<i64>();
                result->dataLength = -(i32)JsonToken_IntegerLiteral;
                break;
            }
            case JsonBinaryField_UInt8:
            {
                *((u8 *)&result->data) = reader->Read<u8>();
                result->dataLength = -(i32)JsonToken_IntegerLiteral;
                break;
            }
            case JsonBinaryField_UInt16:
            {
                *((u16 *)&result->data) = reader->Read<u16>();
                result->dataLength = -(i32)JsonToken_IntegerLiteral;
                break;
            }
            case JsonBinaryField_UInt32:
            {
                *((u32 *)&result->data) = reader->Read<u32>();
                result->dataLength = -(i32)JsonToken_IntegerLiteral;
                break;
            }
            case JsonBinaryField_UInt64:
            {
                *((u64 *)&result->data) = reader->Read<u64>();
                result->dataLength = -(i32)JsonToken_IntegerLiteral;
                break;
            }
            default:
                break;
        }
    }
}

void Json::JsonConvertToBinary(ByteStreamWriter *writer, JsonElement *json)
{
    switch (json->elementType)
    {
        case JsonElement_Object:
        {
            writer->Write((u8)JsonBinaryField_Object);
            writer->Write((u32)json->childObjects.count);
            
            auto iterator = json->childObjects.GetIterator();
            foreach (kvp, iterator)
            {
                writer->WriteString(kvp->key);
                JsonConvertToBinary(writer, &kvp->value);
            }
            break;
        }
        case JsonElement_Array:
        {
            writer->Write((u8)JsonBinaryField_Array);
            writer->Write((u32)json->arrayElements.length);
            for (usize i = 0; i < json->arrayElements.length; i++)
            {
                JsonConvertToBinary(writer, &json->arrayElements.data[i]);
            }
            break;
        }
        case JsonElement_Property:
        {
            Json::JsonTokenType elemType = json->CheckElementType();
            switch (elemType)
            {
                case JsonToken_StringLiteral:
                {
                    writer->WriteByte((u8)JsonBinaryField_String);
                    string reassembled = string();
                    reassembled.buffer = (char *)json->data;
                    reassembled.length = json->dataLength;
                    writer->WriteString(reassembled);
                    //writer->WriteString(json->value);
                    break;
                }
                case JsonToken_FloatLiteral:
                {
                    writer->WriteByte((u8)JsonBinaryField_Float);
                    writer->Write(json->GetFloat());
                    break;
                }
                case JsonToken_BoolLiteral:
                {
                    writer->WriteByte((u8)JsonBinaryField_Bool);
                    writer->Write(json->GetBool());
                    break;
                }
                case JsonToken_NullLiteral:
                {
                    writer->WriteByte((u8)JsonBinaryField_Null);
                    break;
                }
                case JsonToken_UIntegerLiteral:
                {
                    u64 data = json->data;
                    if (data <= U8Max)
                    {
                        writer->WriteByte(JsonBinaryField_UInt8);
                        writer->Write((u8)data);
                    }
                    else if (data <= U16Max)
                    {
                        writer->WriteByte(JsonBinaryField_UInt16);
                        writer->Write((u16)data);
                    }
                    else if (data <= U32Max)
                    {
                        writer->WriteByte(JsonBinaryField_UInt32);
                        writer->Write((u32)data);
                    }
                    else
                    {
                        writer->WriteByte(JsonBinaryField_UInt64);
                        writer->Write(data);
                    }
                    break;
                }
                case JsonToken_IntegerLiteral:
                {
                    i64 data = *(i64 *)&json->data;
                    if (data <= I8Max && data >= I8Min)
                    {
                        writer->WriteByte(JsonBinaryField_Int8);
                        writer->Write((i8)data);
                    }
                    else if (data <= I16Max && data >= I16Min)
                    {
                        writer->WriteByte(JsonBinaryField_Int16);
                        writer->Write((i16)data);
                    }
                    else if (data <= I32Max && data >= I32Min)
                    {
                        writer->WriteByte(JsonBinaryField_Int32);
                        writer->Write((i32)data);
                    }
                    else
                    {
                        writer->WriteByte(JsonBinaryField_Int64);
                        writer->Write(data);
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