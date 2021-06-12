#include "PageMapIndexer.h"

PageMapIndexer::PageMapIndexer(uint64_t VirtualAddress){
    VirtualAddress >>= 12;
    P_i = VirtualAddress & 0x1ff;
    VirtualAddress >>= 9;
    PT_i = VirtualAddress & 0x1ff;
    VirtualAddress >>= 9;
    PD_i = VirtualAddress & 0x1ff;
    VirtualAddress >>= 9;
    PDP_i = VirtualAddress & 0x1ff;
}

