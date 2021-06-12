#pragma once
#include <stdint.h>

class PageMapIndexer {
    public:
        PageMapIndexer(uint64_t VirtualAddress);
        uint64_t PDP_i;
        uint64_t PD_i;
        uint64_t PT_i;
        uint64_t P_i;
};