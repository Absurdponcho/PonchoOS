#pragma once
#include "Paging.h"

class PageTableManager {
    public:
    PageTableManager(PageTable* PML4Address);
    PageTable* PML4;
    void MapMemory(void* virtualMemory, void* physicalMemory);
};

extern PageTableManager GlobalPageTableManager;