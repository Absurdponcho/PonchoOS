#include "kernelUtil.h"
#include "memory/heap.h"

extern "C" void _start(BootInfo* bootInfo){

    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    PageTableManager* pageTableManager = kernelInfo.pageTableManager;

    GlobalRenderer->Print("Kernel Initialized Successfully");

    GlobalRenderer->Next();
    GlobalRenderer->Print(to_hstring((uint64_t)bootInfo->rsdp));  
    GlobalRenderer->Next();

    GlobalRenderer->Print(to_hstring((uint64_t)malloc(0x8000)));
    GlobalRenderer->Next();
    void* address = malloc(0x8000);
    GlobalRenderer->Print(to_hstring((uint64_t)address));
    GlobalRenderer->Next();
    GlobalRenderer->Print(to_hstring((uint64_t)malloc(0x100)));
    GlobalRenderer->Next();

    free(address);

    GlobalRenderer->Print(to_hstring((uint64_t)malloc(0x8001)));
    GlobalRenderer->Next();

    while(true);

}