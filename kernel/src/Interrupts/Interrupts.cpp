#include "Interrupts.h"
#include "../Panic.h"
#include "../IO.h"
#include "../UserInput/Keyboard.h"
#include "../Scheduling/PIT/PIT.h"

__attribute__((interrupt)) void PageFault_Handler(InterruptFrame* Frame){
    Panic("Page Fault Detected");
    while(true);
}

__attribute__((interrupt)) void DoubleFault_Handler(InterruptFrame* Frame){
    Panic("Double Fault Detected");
    while(true);
}

__attribute__((interrupt)) void GPFault_Handler(InterruptFrame* Frame){
    Panic("General Protection Fault Detected");
    while(true);
}



__attribute__((interrupt)) void KeyboardInt_Handler(InterruptFrame* Frame){
    uint8_t Scancode = InByte(0x60);

    HandleKeyboard(Scancode);

    PIC_EndMaster();
}

__attribute__((interrupt)) void MouseInt_Handler(InterruptFrame* Frame){

    uint8_t MouseData = InByte(0x60);

    HandlePS2Mouse(MouseData);

    PIC_EndSlave();
}

__attribute__((interrupt)) void PITInt_Handler(InterruptFrame* Frame){
    PIT::Tick();
    PIC_EndMaster();
}

void PIC_EndMaster(){
    OutByte(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave(){
    OutByte(PIC2_COMMAND, PIC_EOI);
    OutByte(PIC1_COMMAND, PIC_EOI);
}
   

void RemapPIC(){
    uint8_t a1, a2; 

    a1 = InByte(PIC1_DATA);
    IOWait();
    a2 = InByte(PIC2_DATA);
    IOWait();

    OutByte(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    IOWait();
    OutByte(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    IOWait();

    OutByte(PIC1_DATA, 0x20);
    IOWait();
    OutByte(PIC2_DATA, 0x28);
    IOWait();

    OutByte(PIC1_DATA, 4);
    IOWait();
    OutByte(PIC2_DATA, 2);
    IOWait();

    OutByte(PIC1_DATA, ICW4_8086);
    IOWait();
    OutByte(PIC2_DATA, ICW4_8086);
    IOWait();

    OutByte(PIC1_DATA, a1);
    IOWait();
    OutByte(PIC2_DATA, a2);

}