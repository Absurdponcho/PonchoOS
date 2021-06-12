#pragma once

#include <stdint.h>
#include "EFIMemory.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* MemoryMapFirstDescriptor, uint64_t MemoryMapEntries, uint64_t MemoryMapDescriptorSize);
void Memset(void* Start, uint8_t Value, uint64_t Num);