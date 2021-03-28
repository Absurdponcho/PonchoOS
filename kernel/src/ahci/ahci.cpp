#include "ahci.h"
#include "../BasicRenderer.h"
#include "../paging/PageTableManager.h"
#include "../memory/heap.h"
#include "../paging/PageFrameAllocator.h"

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

    PortType CheckPortType(HBAPort* port){
        uint32_t sataStatus = port->sataStatus;

        uint8_t interfacePowerManagement = (sataStatus >> 8) & 0b111;
        uint8_t deviceDetection = sataStatus & 0b111;

        if (deviceDetection != HBA_PORT_DEV_PRESENT) return PortType::None;
        if (interfacePowerManagement != HBA_PORT_IPM_ACTIVE) return PortType::None;

        switch (port->signature){
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
        uint32_t portsImplemented = ABAR->portsImplemented;
        for (int i = 0; i < 32; i++){
            if (portsImplemented & (1 << i)){
                PortType portType = CheckPortType(&ABAR->ports[i]);

                if (portType == PortType::SATA || portType == PortType::SATAPI){
                    ports[portCount] = new Port();
                    ports[portCount]->portType = portType;
                    ports[portCount]->hbaPort = &ABAR->ports[i];
                    ports[portCount]->portNumber = portCount;
                    portCount++;
                }
            }
        }
    }

    void Port::Configure(){
        StopCMD();

        void* newBase = GlobalAllocator.RequestPage();
        hbaPort->commandListBase = (uint32_t)(uint64_t)newBase;
        hbaPort->commandListBaseUpper = (uint32_t)((uint64_t)newBase >> 32);
        memset((void*)(hbaPort->commandListBase), 0, 1024);

        void* fisBase = GlobalAllocator.RequestPage();
        hbaPort->fisBaseAddress = (uint32_t)(uint64_t)fisBase;
        hbaPort->fisBaseAddressUpper = (uint32_t)((uint64_t)fisBase >> 32);
        memset(fisBase, 0, 256);

        HBACommandHeader* cmdHeader = (HBACommandHeader*)((uint64_t)hbaPort->commandListBase + ((uint64_t)hbaPort->commandListBaseUpper << 32));

        for (int i = 0; i < 32; i++){
            cmdHeader[i].prdtLength = 8;

            void* cmdTableAddress = GlobalAllocator.RequestPage();
            uint64_t address = (uint64_t)cmdTableAddress + (i << 8);
            cmdHeader[i].commandTableBaseAddress = (uint32_t)(uint64_t)address;
            cmdHeader[i].commandTableBaseAddressUpper = (uint32_t)((uint64_t)address >> 32);
            memset(cmdTableAddress, 0, 256);
        }

        StartCMD();
    }

    void Port::StopCMD(){
        hbaPort->cmdSts &= ~HBA_PxCMD_ST;
        hbaPort->cmdSts &= ~HBA_PxCMD_FRE;

        while(true){
            if (hbaPort->cmdSts & HBA_PxCMD_FR) continue;
            if (hbaPort->cmdSts & HBA_PxCMD_CR) continue;

            break;
        }

    }

    void Port::StartCMD(){
        while (hbaPort->cmdSts & HBA_PxCMD_CR);

        hbaPort->cmdSts |= HBA_PxCMD_FRE;
        hbaPort->cmdSts |= HBA_PxCMD_ST;
    }

    bool Port::Read(uint64_t sector, uint32_t sectorCount, void* buffer){
        uint32_t sectorL = (uint32_t) sector;
        uint32_t sectorH = (uint32_t) (sector >> 32);

        hbaPort->interruptStatus = (uint32_t)-1; // Clear pending interrupt bits

        HBACommandHeader* cmdHeader = (HBACommandHeader*)hbaPort->commandListBase;
        cmdHeader->commandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
        cmdHeader->write = 0; //this is a read
        cmdHeader->prdtLength = 1;

        HBACommandTable* commandTable = (HBACommandTable*)(cmdHeader->commandTableBaseAddress);
        memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength-1)*sizeof(HBAPRDTEntry));

        commandTable->prdtEntry[0].dataBaseAddress = (uint32_t)(uint64_t)buffer;
        commandTable->prdtEntry[0].dataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
        commandTable->prdtEntry[0].byteCount = (sectorCount<<9)-1; // 512 bytes per sector
        commandTable->prdtEntry[0].interruptOnCompletion = 1;

        FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->commandFIS);

        cmdFIS->fisType = FIS_TYPE_REG_H2D;
        cmdFIS->commandControl = 1; // command
        cmdFIS->command = ATA_CMD_READ_DMA_EX;

        cmdFIS->lba0 = (uint8_t)sectorL;
        cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
        cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
        cmdFIS->lba3 = (uint8_t)sectorH;
        cmdFIS->lba4 = (uint8_t)(sectorH >> 8);
        cmdFIS->lba4 = (uint8_t)(sectorH >> 16);

        cmdFIS->deviceRegister = 1<<6; //LBA mode

        cmdFIS->countLow = sectorCount & 0xFF;
        cmdFIS->countHigh = (sectorCount >> 8) & 0xFF;

        uint64_t spin = 0;

        while ((hbaPort->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000){
            spin ++;
        }
        if (spin == 1000000) {
            return false;
        }

        hbaPort->commandIssue = 1;

        while (true){

            if((hbaPort->commandIssue == 0)) break;
            if(hbaPort->interruptStatus & HBA_PxIS_TFES)
            {
                return false;
            }
        }

        return true;
    }

    AHCIDriver::AHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress){
        this->PCIBaseAddress = pciBaseAddress;
        GlobalRenderer->Print("AHCI Driver instance initialized");
        GlobalRenderer->Next();

        ABAR = (HBAMemory*)((PCI::PCIHeader0*)pciBaseAddress)->BAR5;

        g_PageTableManager.MapMemory(ABAR, ABAR);
        ProbePorts();
        
        for (int i = 0; i < portCount; i++){
            Port* port = ports[i];

            port->Configure();

            port->buffer = (uint8_t*)GlobalAllocator.RequestPage();
            memset(port->buffer, 0, 0x1000);

            port->Read(0, 4, port->buffer);
            for (int t = 0; t < 1024; t++){
                GlobalRenderer->PutChar(port->buffer[t]);
            }
            GlobalRenderer->Next();
        }
    }

    AHCIDriver::~AHCIDriver(){

    }
}