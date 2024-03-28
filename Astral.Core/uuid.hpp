#pragma once
#include "random.hpp"
#include "hash.hpp"

struct uuid
{
    u8 byte[16];

    inline static uuid New(Random* random)
    {
        uuid result;
        u64* asPointer = (u64*)&result;
        asPointer[0] = random->Next();
        asPointer[1] = random->Next();

        result.byte[6] = (result.byte[6] & 0x0f) | 0x40;
        result.byte[8] = (result.byte[8] & 0x3f) | 0x80;

        return result;
    }

    inline bool operator==(uuid other)
    {
        u64* asPointer = (u64*)this;
        u64* otherAsPointer = (u64*)&other;
        return asPointer[0] == otherAsPointer[0] && asPointer[1] == otherAsPointer[1];
    }
    inline bool operator!=(uuid other)
    {
        u64* asPointer = (u64*)this;
        u64* otherAsPointer = (u64*)&other;
        return asPointer[0] != otherAsPointer[0] || asPointer[1] != otherAsPointer[1];
    }
    inline bool Equals(void* other)
    {
        //lol
        u64* asPointer = (u64*)this;
        u64* otherAsPointer = (u64*)other;
        return asPointer[0] == otherAsPointer[0] && asPointer[1] == otherAsPointer[1];
    }
    inline u64 GetHashCode()
    {
        return Murmur2((u8*)this, 16);
    }
};