#include "IDT.h"

void IDTDescEntry::SetOffset(uint64_t Offset){
    Offset0 = (uint16_t)(Offset & 0x000000000000ffff);
    Offset1 = (uint16_t)((Offset & 0x00000000ffff0000) >> 16);
    Offset2 = (uint32_t)((Offset & 0xffffffff00000000) >> 32);
}

uint64_t IDTDescEntry::GetOffset(){ 
    uint64_t Offset = 0;
    Offset |= (uint64_t)Offset0;
    Offset |= (uint64_t)Offset1 << 16;
    Offset |= (uint64_t)Offset2 << 32;
    return Offset;
}