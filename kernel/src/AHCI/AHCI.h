#pragma once
#include <stdint.h>
#include "../PCI.h"

namespace AHCI {

    #define ATA_DEV_BUSY 0x80
    #define ATA_DEV_DRQ 0x08
    #define ATA_CMD_READ_DMA_EX 0x25

    #define HBA_PxIS_TFES (1 << 30)

    enum PortType {
        None = 0,
        SATA = 1,
        SEMB = 2,
        PM = 3,
        SATAPI = 4,
    };

    enum FIS_TYPE{
        FIS_TYPE_REG_H2D = 0x27,
        FIS_TYPE_REG_D2H = 0x34,
        FIS_TYPE_DMA_ACT = 0x39,
        FIS_TYPE_DMA_SETUP = 0x41,
        FIS_TYPE_DATA = 0x46,
        FIS_TYPE_BIST = 0x58,
        FIS_TYPE_PIO_SETUP = 0x5F,
        FIS_TYPE_DEV_BITS = 0xA1,
    };

    struct HBAPort{
        uint32_t CommandListBase;
        uint32_t CommandListBaseUpper;
        uint32_t FISBaseAddress;
        uint32_t FISBaseAddressUpper;
        uint32_t InterruptStatus;
        uint32_t InterruptEnable;
        uint32_t CommandStatus;
        uint32_t Reserved0;
        uint32_t TaskFileData;
        uint32_t Signature;
        uint32_t SataStatus;
        uint32_t SataControl;
        uint32_t SataError;
        uint32_t SataActive;
        uint32_t CommandIssue;
        uint32_t SataNotification;
        uint32_t FISSwitchControl;
        uint32_t Reserved1[11];
        uint32_t Vendor[4];
    };

    struct HBAMemory{
        uint32_t HostCapability;
        uint32_t GlobalHostControl;
        uint32_t InterruptStatus;
        uint32_t PortsImplemented;
        uint32_t Version;
        uint32_t CCCControl;
        uint32_t CCCPorts;
        uint32_t EnclosureManagementLocation;
        uint32_t EnclosureManagementControl;
        uint32_t HostCapabilitiesExtended;
        uint32_t BIOSHandoffControlStatus;
        uint8_t Reserved0[0x74];
        uint8_t Vendor[0x60];
        HBAPort Ports[1];
    };

    struct HBACommandHeader {
        uint8_t CommandFISLength:5;
        uint8_t ATAPI:1;
        uint8_t Write:1;
        uint8_t Prefetchable:1;

        uint8_t Reset:1;
        uint8_t BIST:1;
        uint8_t ClearBusy:1;
        uint8_t Reserved0:1;
        uint8_t PortMultiplier:4;

        uint16_t PRDTLength;
        uint32_t PRDBCount;
        uint32_t CommandTableBaseAddress;
        uint32_t CommandTableBaseAddressUpper;
        uint32_t Reserved1[4];
    };

    struct HBAPRDTEntry{
        uint32_t DataBaseAddress;
        uint32_t DataBaseAddressUpper;
        uint32_t Reserved0;

        uint32_t ByteCount:22;
        uint32_t Reserved1:9;
        uint32_t InterruptOnCompletion:1;
    };

    struct HBACommandTable{
        uint8_t CommandFIS[64];

        uint8_t ATAPICommand[16];

        uint8_t Reserved[48];

        HBAPRDTEntry PRDTEntry[];
    };

    struct FIS_REG_H2D {
        uint8_t FISType;

        uint8_t PortMultiplier:4;
        uint8_t Reserved0:3;
        uint8_t CommandControl:1;

        uint8_t Command;
        uint8_t FeatureLow;

        uint8_t LBA0;
        uint8_t LBA1;
        uint8_t LBA2;
        uint8_t DeviceRegister;

        uint8_t LBA3;
        uint8_t LBA4;
        uint8_t LBA5;
        uint8_t FeatureHigh;

        uint8_t CountLow;
        uint8_t CountHigh;
        uint8_t ISOCommandCompletion;
        uint8_t Control;

        uint8_t Reserved1[4];
    }; 

    class Port {
        public:
            HBAPort* HBAPortPtr;
            PortType AHCIPortType;
            uint8_t* Buffer;
            uint8_t PortNumber;
            void Configure();
            void StartCMD(); 
            void StopCMD();
            bool Read(uint64_t Sector, uint32_t SectorCount, void* Buffer);
    };

    class AHCIDriver{
        public:
        AHCIDriver(PCI::PCIDeviceHeader* PCIBaseAddress);
        ~AHCIDriver();
        PCI::PCIDeviceHeader* PCIBaseAddress;
        HBAMemory* ABAR;
        void ProbePorts();
        Port* Ports[32];
        uint8_t PortCount;
    };
}