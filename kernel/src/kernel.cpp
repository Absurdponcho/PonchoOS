#include "kernelUtil.h"

extern "C" void _start(BootInfo* bootInfo){

    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    PageTableManager* pageTableManager = kernelInfo.pageTableManager;

    GlobalRenderer->Print("Kernel Initialized Successfully");

    int* test = (int*)0x80000000000;
    *test = 2;

    while(true);
}