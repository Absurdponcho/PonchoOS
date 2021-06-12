#pragma once

#include <stdint.h>
#include "BasicRenderer.h"
#include "CStr.h"
#include "EFIMemory.h"
#include "Memory.h"
#include "Bitmap.h"
#include "Paging/PageFrameAllocator.h"
#include "Paging/PageMapIndexer.h"
#include "Paging/Paging.h"
#include "Paging/PageTableManager.h"
#include "UserInput/Mouse.h"
#include "ACPI.h"
#include "PCI.h"

struct BootInfo {
	Framebuffer* BootFramebuffer;
	PSF1_FONT* PSF1Font;
	EFI_MEMORY_DESCRIPTOR* MemoryMapFirstDescriptor;
	uint64_t MemoryMapSize;
	uint64_t MemoryMapDescriptorSize;
	ACPI::RSDP2* RSDP;
} ;

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

struct KernelInfo {
    PageTableManager* KernelPageTableManager;
};

KernelInfo InitializeKernel(BootInfo* BootInfo);