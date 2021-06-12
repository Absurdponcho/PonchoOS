#pragma once
#include <stdint.h>

struct EFI_MEMORY_DESCRIPTOR {
    uint32_t Type;
    void* PhysicalAddress;
    void* virtAddr; 
    uint64_t NumPages;
    uint64_t attribs;
};

extern const char* EFI_MEMORY_TYPE_STRINGS[];