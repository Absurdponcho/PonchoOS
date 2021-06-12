#pragma once
#include "../BasicRenderer.h"
#include "../UserInput/Mouse.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

struct InterruptFrame;
__attribute__((interrupt)) void PageFault_Handler(InterruptFrame* Frame);
__attribute__((interrupt)) void DoubleFault_Handler(InterruptFrame* Frame);
__attribute__((interrupt)) void GPFault_Handler(InterruptFrame* Frame);
__attribute__((interrupt)) void KeyboardInt_Handler(InterruptFrame* Frame);
__attribute__((interrupt)) void MouseInt_Handler(InterruptFrame* Frame);
__attribute__((interrupt)) void PITInt_Handler(InterruptFrame* Frame);

void RemapPIC();
void PIC_EndMaster();
void PIC_EndSlave();