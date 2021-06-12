#pragma once
#include <stdint.h>

enum PT_Flag {
    Present = 0,
    ReadWrite = 1,
    UserSuper = 2,
    WriteThrough = 3,
    CacheDisabled = 4,
    Accessed = 5,
    LargerPages = 7,
    Custom0 = 9,
    Custom1 = 10,
    Custom2 = 11,
    NX = 63 // only if supported
};

struct PageDirectoryEntry {
    uint64_t Value;
    void SetFlag(PT_Flag Flag, bool Enabled);
    bool GetFlag(PT_Flag Flag);
    void SetAddress(uint64_t Address);
    uint64_t GetAddress();
};

struct PageTable { 
    PageDirectoryEntry Entries [512];
}__attribute__((aligned(0x1000)));