/*#pragma once
#include "hashmap.hpp"
#include "string.hpp"

template<typename T>
struct BinaryConverter
{
    def_delegate(ToBinaryFunction, u8*, IAllocator, T);
    static ToBinaryFunction ToBinary;
};

struct BtonCompound
{
    IAllocator allocator;
    collections::hashmap<string, u8 *> properties;

    inline BtonCompound()
    {
        allocator = IAllocator{};
        properties = collections::hashmap<string, u8 *>();
    }
    inline BtonCompound(IAllocatorallocator)
    {
        this->allocator = allocator;
        properties = collections::hashmap<string, u8 *>(allocator, &stringHash, &stringEql);
    }
};*/