#include "Paging.h"

void PageDirectoryEntry::SetFlag(PT_Flag Flag, bool Enabled){
    uint64_t BitSelector = (uint64_t)1 << Flag;
    Value &= ~BitSelector;
    if (Enabled){
        Value |= BitSelector;
    }
}

bool PageDirectoryEntry::GetFlag(PT_Flag Flag){
    uint64_t BitSelector = (uint64_t)1 << Flag;
    return Value & BitSelector > 0 ? true : false;
}

uint64_t PageDirectoryEntry::GetAddress(){
    return (Value & 0x000ffffffffff000) >> 12;
}

void PageDirectoryEntry::SetAddress(uint64_t Address){
    Address &= 0x000000ffffffffff;
    Value &= 0xfff0000000000fff;
    Value |= (Address << 12);
}