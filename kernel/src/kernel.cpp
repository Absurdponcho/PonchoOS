#include "kernelUtil.h"
#include "memory/heap.h"
#include "scheduling/pit/pit.h"

extern "C" void _start(BootInfo* bootInfo){

    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    
    while(true){
        asm ("hlt");
    }

}