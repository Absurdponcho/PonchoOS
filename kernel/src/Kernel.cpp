#include "KernelUtil.h"
#include "Memory/Heap.h"
#include "Scheduling/PIT/PIT.h"

extern "C" void KernelStart(BootInfo* BootInfo){

    KernelInfo KernelInfo = InitializeKernel(BootInfo);
    

    while(true){
        asm ("hlt");
    }

}