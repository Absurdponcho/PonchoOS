#pragma once
#include <stdint.h>

struct GDTDescriptor {
    uint16_t Size;
    uint64_t Offset;
} __attribute__((packed));

struct GDTEntry {
    uint16_t Limit0;
    uint16_t Base0;
    uint8_t Base1;
    uint8_t AccessByte;
    uint8_t Limit1_Flags;
    uint8_t Base2;
}__attribute__((packed));

struct GDT {
    GDTEntry Null; //0x00
    GDTEntry KernelCode; //0x08
    GDTEntry KernelData; //0x10
    GDTEntry UserNull;
    GDTEntry UserCode;
    GDTEntry UserData;
} __attribute__((packed)) 
__attribute((aligned(0x1000)));

extern GDT DefaultGDT;

extern "C" void LoadGDT(GDTDescriptor* GDTDescriptor);