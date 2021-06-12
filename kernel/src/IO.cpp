#include "IO.h"

void OutByte(uint16_t Port, uint8_t Value){
    asm volatile ("outb %0, %1" : : "a"(Value), "Nd"(Port));
}

uint8_t InByte(uint16_t Port){
    uint8_t ReturnValue;
    asm volatile ("inb %1, %0"
    : "=a"(ReturnValue)
    : "Nd"(Port));
    return ReturnValue;
}

void IOWait(){
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}