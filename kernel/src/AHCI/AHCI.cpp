#include "AHCI.h"
#include "../BasicRenderer.h"
#include "../Paging/PageTableManager.h"
#include "../Memory/Heap.h"
#include "../Paging/PageFrameAllocator.h"

namespace AHCI{

    #define HBA_PORT_DEV_PRESENT 0x3
    #define HBA_PORT_IPM_ACTIVE 0x1
    #define SATA_SIG_ATAPI 0xEB140101
    #define SATA_SIG_ATA 0x00000101
    #define SATA_SIG_SEMB 0xC33C0101
    #define SATA_SIG_PM 0x96690101

    #define HBA_PxCMD_CR 0x8000
    #define HBA_PxCMD_FRE 0x0010
    #define HBA_PxCMD_ST 0x0001
    #define HBA_PxCMD_FR 0x4000

    PortType CheckPortType(HBAPort* Port){
        uint32_t SataStatus = Port->SataStatus;

        uint8_t InterfacePowerManagement = (SataStatus >> 8) & 0b111;
        uint8_t DeviceDetection = SataStatus & 0b111;

        if (DeviceDetection != HBA_PORT_DEV_PRESENT) return PortType::None;
        if (InterfacePowerManagement != HBA_PORT_IPM_ACTIVE) return PortType::None;

        switch (Port->Signature){
            case SATA_SIG_ATAPI:
                return PortType::SATAPI;
            case SATA_SIG_ATA:
                return PortType::SATA;
            case SATA_SIG_PM:
                return PortType::PM;
            case SATA_SIG_SEMB:
                return PortType::SEMB;
            default:
                PortType::None;
        }
    }

    void AHCIDriver::ProbePorts(){
        uint32_t PortsImplemented = ABAR->PortsImplemented;
        for (int i = 0; i < 32; i++){
            if (PortsImplemented & (1 << i)){
                PortType portType = CheckPortType(&ABAR->Ports[i]);

                if (portType == PortType::SATA || portType == PortType::SATAPI){
                    Ports[PortCount] = new Port();
                    Ports[PortCount]->AHCIPortType = portType;
                    Ports[PortCount]->HBAPortPtr = &ABAR->Ports[i];
                    Ports[PortCount]->PortNumber = PortCount;
                    PortCount++;
                }
            }
        }
    }

    void Port::Configure(){
        StopCMD();

        void* NewBase = GlobalAllocator.RequestPage();
        HBAPortPtr->CommandListBase = (uint32_t)(uint64_t)NewBase;
        HBAPortPtr->CommandListBaseUpper = (uint32_t)((uint64_t)NewBase >> 32);
        Memset((void*)(HBAPortPtr->CommandListBase), 0, 1024);

        void* FISBase = GlobalAllocator.RequestPage();
        HBAPortPtr->FISBaseAddress = (uint32_t)(uint64_t)FISBase;
        HBAPortPtr->FISBaseAddressUpper = (uint32_t)((uint64_t)FISBase >> 32);
        Memset(FISBase, 0, 256);

        HBACommandHeader* CommandHeader = (HBACommandHeader*)((uint64_t)HBAPortPtr->CommandListBase + ((uint64_t)HBAPortPtr->CommandListBaseUpper << 32));

        for (int i = 0; i < 32; i++){
            CommandHeader[i].PRDTLength = 8;

            void* CommandTableAddress = GlobalAllocator.RequestPage();
            uint64_t Address = (uint64_t)CommandTableAddress + (i << 8);
            CommandHeader[i].CommandTableBaseAddress = (uint32_t)(uint64_t)Address;
            CommandHeader[i].CommandTableBaseAddressUpper = (uint32_t)((uint64_t)Address >> 32);
            Memset(CommandTableAddress, 0, 256);
        }

        StartCMD();
    }

    void Port::StopCMD(){
        HBAPortPtr->CommandStatus &= ~HBA_PxCMD_ST;
        HBAPortPtr->CommandStatus &= ~HBA_PxCMD_FRE;

        while(true){
            if (HBAPortPtr->CommandStatus & HBA_PxCMD_FR) continue;
            if (HBAPortPtr->CommandStatus & HBA_PxCMD_CR) continue;

            break;
        }

    }

    void Port::StartCMD(){
        while (HBAPortPtr->CommandStatus & HBA_PxCMD_CR);

        HBAPortPtr->CommandStatus |= HBA_PxCMD_FRE;
        HBAPortPtr->CommandStatus |= HBA_PxCMD_ST;
    }

    bool Port::Read(uint64_t Sector, uint32_t SectorCount, void* Buffer){
        uint32_t SectorL = (uint32_t) Sector;
        uint32_t SectorH = (uint32_t) (Sector >> 32);

        HBAPortPtr->InterruptStatus = (uint32_t)-1; // Clear pending interrupt bits

        HBACommandHeader* CommandHeader = (HBACommandHeader*)HBAPortPtr->CommandListBase;
        CommandHeader->CommandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
        CommandHeader->Write = 0; //this is a read
        CommandHeader->PRDTLength = 1;

        HBACommandTable* CommandTable = (HBACommandTable*)(CommandHeader->CommandTableBaseAddress);
        Memset(CommandTable, 0, sizeof(HBACommandTable) + (CommandHeader->PRDTLength-1)*sizeof(HBAPRDTEntry));

        CommandTable->PRDTEntry[0].DataBaseAddress = (uint32_t)(uint64_t)Buffer;
        CommandTable->PRDTEntry[0].DataBaseAddressUpper = (uint32_t)((uint64_t)Buffer >> 32);
        CommandTable->PRDTEntry[0].ByteCount = (SectorCount<<9)-1; // 512 bytes per sector
        CommandTable->PRDTEntry[0].InterruptOnCompletion = 1;

        FIS_REG_H2D* CommandFIS = (FIS_REG_H2D*)(&CommandTable->CommandFIS);

        CommandFIS->FISType = FIS_TYPE_REG_H2D;
        CommandFIS->CommandControl = 1; // command
        CommandFIS->Command = ATA_CMD_READ_DMA_EX;

        CommandFIS->LBA0 = (uint8_t)SectorL;
        CommandFIS->LBA1 = (uint8_t)(SectorL >> 8);
        CommandFIS->LBA2 = (uint8_t)(SectorL >> 16);
        CommandFIS->LBA3 = (uint8_t)SectorH;
        CommandFIS->LBA4 = (uint8_t)(SectorH >> 8);
        CommandFIS->LBA4 = (uint8_t)(SectorH >> 16);

        CommandFIS->DeviceRegister = 1<<6; //LBA mode

        CommandFIS->CountLow = SectorCount & 0xFF;
        CommandFIS->CountHigh = (SectorCount >> 8) & 0xFF;

        uint64_t Spin = 0;

        while ((HBAPortPtr->TaskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && Spin < 1000000){
            Spin ++;
        }
        if (Spin == 1000000) {
            return false;
        }

        HBAPortPtr->CommandIssue = 1;

        while (true){

            if((HBAPortPtr->CommandIssue == 0)) break;
            if(HBAPortPtr->InterruptStatus & HBA_PxIS_TFES)
            {
                return false;
            }
        }

        return true;
    }

    AHCIDriver::AHCIDriver(PCI::PCIDeviceHeader* PCIBaseAddress){
        this->PCIBaseAddress = PCIBaseAddress;
        GlobalRenderer->Print("AHCI Driver instance initialized");
        GlobalRenderer->Next();

        ABAR = (HBAMemory*)((PCI::PCIHeader0*)PCIBaseAddress)->BAR5;

        GlobalPageTableManager.MapMemory(ABAR, ABAR);
        ProbePorts();
        
        for (int i = 0; i < PortCount; i++){
            Port* Port = Ports[i];

            Port->Configure();

            Port->Buffer = (uint8_t*)GlobalAllocator.RequestPage();
            Memset(Port->Buffer, 0, 0x1000);

            Port->Read(0, 4, Port->Buffer);
            for (int t = 0; t < 1024; t++){
                GlobalRenderer->PutChar(Port->Buffer[t]);
            }
            GlobalRenderer->Next();
        }
    }

    AHCIDriver::~AHCIDriver(){

    }
}