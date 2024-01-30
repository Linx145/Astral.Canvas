#include "Json.hpp"

void SomnialJson::JsonElement::DumpJsonToStdout(i32 indents)
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
SomnialJson::JsonToken SomnialJson::JsonTokenizer::Next()
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
bool SomnialJson::ParseJsonElement(IAllocator *allocator, JsonTokenizer *tokenizer, JsonElement *result)
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

        collections::vector<JsonElement> arrayMembers = collections::vector<JsonElement>(&cAllocator);
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

            if (tokenizer->PeekNext().tokenType == JsonToken_Comma)
            {
                tokenizer->Next();
                continue;
            }
        }
        return true;
    }
    return false;
}