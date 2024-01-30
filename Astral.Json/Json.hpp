#pragma once
#include "string.hpp"
#include "allocators.hpp"
#include "ctype.h"
#include "hashmap.hpp"
#include "io.hpp"
#include "stdio.h"

namespace SomnialJson
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
        inline string GetString(IAllocator *allocator, JsonToken token)
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
        
        inline i32 GetInt32()
        {
            if (elementType != JsonElement_Property)
            {
                return 0;
            }

            i32 result = atoi(value.buffer);

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
        inline string GetString(IAllocator *allocator)
        {
            return string(allocator, value.buffer);
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
                
                string propertyName = string(&allocator, ptr);
                JsonElement *result = childObjects.Get(propertyName);
                propertyName.deinit();
                return result;
            }
            return NULL;
        }
        void DumpJsonToStdout(i32 indents);
    };
    bool ParseJsonElement(IAllocator *allocator, JsonTokenizer *tokenizer, JsonElement *result);
    inline usize ParseJsonDocument(IAllocator *allocator, string contents, JsonElement* result)
    {
        JsonTokenizer tokenizer = JsonTokenizer(contents);
        if (!ParseJsonElement(allocator, &tokenizer, result))
        {
            return tokenizer.currentLine;
        }
        return 0;
    }
    inline usize ReadJsonFile(IAllocator *allocator, string fileFullPath, JsonElement* result)
    {
        IAllocator cAllocator = GetCAllocator();
        string fileContents = io::ReadFile(&cAllocator, fileFullPath.buffer);
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

        inline JsonWriter(IAllocator *allocator, FILE *fileStream, bool writerShouldIndent)
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