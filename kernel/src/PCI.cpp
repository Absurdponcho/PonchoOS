#include "PCI.h"
#include "AHCI/AHCI.h"
#include "Memory/Heap.h"

namespace PCI{

    void EnumerateFunction(uint64_t DeviceAddress, uint64_t Function){
        uint64_t Offset = Function << 12;

        uint64_t FunctionAddress = DeviceAddress + Offset;
        GlobalPageTableManager.MapMemory((void*)FunctionAddress, (void*)FunctionAddress);

        PCIDeviceHeader* PCIDeviceHdr = (PCIDeviceHeader*)FunctionAddress;

        if (PCIDeviceHdr->DeviceID == 0) return;
        if (PCIDeviceHdr->DeviceID == 0xFFFF) return;

        GlobalRenderer->Print(GetVendorName(PCIDeviceHdr->VendorID));
        GlobalRenderer->Print(" / ");
        GlobalRenderer->Print(GetDeviceName(PCIDeviceHdr->VendorID, PCIDeviceHdr->DeviceID));
        GlobalRenderer->Print(" / ");
        GlobalRenderer->Print(DeviceClasses[PCIDeviceHdr->Class]);
        GlobalRenderer->Print(" / ");
        GlobalRenderer->Print(GetSubclassName(PCIDeviceHdr->Class, PCIDeviceHdr->Subclass));
        GlobalRenderer->Print(" / ");
        GlobalRenderer->Print(GetProgIFName(PCIDeviceHdr->Class, PCIDeviceHdr->Subclass, PCIDeviceHdr->ProgIF));
        GlobalRenderer->Next();

        switch (PCIDeviceHdr->Class){
            case 0x01: // mass storage controller
                switch (PCIDeviceHdr->Subclass){
                    case 0x06: //Serial ATA 
                        switch (PCIDeviceHdr->ProgIF){
                            case 0x01: //AHCI 1.0 Device
                                new AHCI::AHCIDriver(PCIDeviceHdr);
                        }
                }
        }
    }

    void EnumerateDevice(uint64_t BusAddress, uint64_t Device){
        uint64_t Offset = Device << 15;

        uint64_t DeviceAddress = BusAddress + Offset;
        GlobalPageTableManager.MapMemory((void*)DeviceAddress, (void*)DeviceAddress);

        PCIDeviceHeader* PCIDeviceHdr = (PCIDeviceHeader*)DeviceAddress;

        if (PCIDeviceHdr->DeviceID == 0) return;
        if (PCIDeviceHdr->DeviceID == 0xFFFF) return;

        for (uint64_t Function = 0; Function < 8; Function++){
            EnumerateFunction(DeviceAddress, Function);
        }
    }

    void EnumerateBus(uint64_t BaseAddress, uint64_t Bus){
        uint64_t Offset = Bus << 20;

        uint64_t BusAddress = BaseAddress + Offset;
        GlobalPageTableManager.MapMemory((void*)BusAddress, (void*)BusAddress);

        PCIDeviceHeader* PCIDeviceHdr = (PCIDeviceHeader*)BusAddress;

        if (PCIDeviceHdr->DeviceID == 0) return;
        if (PCIDeviceHdr->DeviceID == 0xFFFF) return;

        for (uint64_t Device = 0; Device < 32; Device++){
            EnumerateDevice(BusAddress, Device);
        }
    }

    void EnumeratePCI(ACPI::MCFGHeader* MCFG){
        int Entries = ((MCFG->Header.Length) - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);
        for (int t = 0; t < Entries; t++){
            ACPI::DeviceConfig* NewDeviceConfig = (ACPI::DeviceConfig*)((uint64_t)MCFG + sizeof(ACPI::MCFGHeader) + (sizeof(ACPI::DeviceConfig) * t));
            for (uint64_t Bus = NewDeviceConfig->StartBus; Bus < NewDeviceConfig->EndBus; Bus++){
                EnumerateBus(NewDeviceConfig->BaseAddress, Bus);
            }
        }
    }

}