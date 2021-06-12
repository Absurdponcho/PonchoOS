#include <stdint.h>
#include "CStr.h"

namespace PCI {
    const char* DeviceClasses[] {
        "Unclassified",
        "Mass Storage Controller",
        "Network Controller",
        "Display Controller",
        "Multimedia Controller",
        "Memory Controller",
        "Bridge Device",
        "Simple Communication Controller",
        "Base System Peripheral",
        "Input Device Controller",
        "Docking Station", 
        "Processor",
        "Serial Bus Controller",
        "Wireless Controller",
        "Intelligent Controller",
        "Satellite Communication Controller",
        "Encryption Controller",
        "Signal Processing Controller",
        "Processing Accelerator",
        "Non Essential Instrumentation"
    };

    const char* GetVendorName(uint16_t VendorID){
        switch (VendorID){
            case 0x8086:
                return "Intel Corp";
            case 0x1022:
                return "AMD";
            case 0x10DE:
                return "NVIDIA Corporation";
        }
        return ToHexString(VendorID);
    }

    const char* GetDeviceName(uint16_t VendorID, uint16_t DeviceID){
        switch (VendorID){
            case 0x8086: // Intel
                switch(DeviceID){
                    case 0x29C0:
                        return "Express DRAM Controller";
                    case 0x2918:
                        return "LPC Interface Controller";
                    case 0x2922:
                        return "6 port SATA Controller [AHCI mode]";
                    case 0x2930:
                        return "SMBus Controller";
                }
        }
        return ToHexString(DeviceID);
    }

    const char* MassStorageControllerSubclassName(uint8_t SubclassCode){
        switch (SubclassCode){
            case 0x00:
                return "SCSI Bus Controller";
            case 0x01:
                return "IDE Controller";
            case 0x02:
                return "Floppy Disk Controller";
            case 0x03:
                return "IPI Bus Controller";
            case 0x04:
                return "RAID Controller";
            case 0x05:
                return "ATA Controller";
            case 0x06:
                return "Serial ATA";
            case 0x07:
                return "Serial Attached SCSI";
            case 0x08:
                return "Non-Volatile Memory Controller";
            case 0x80:
                return "Other";
        }
        return ToHexString(SubclassCode);
    }

    const char* SerialBusControllerSubclassName(uint8_t SubclassCode){
        switch (SubclassCode){
            case 0x00:
                return "FireWire (IEEE 1394) Controller";
            case 0x01:
                return "ACCESS Bus";
            case 0x02:
                return "SSA";
            case 0x03:
                return "USB Controller";
            case 0x04:
                return "Fibre Channel";
            case 0x05:
                return "SMBus";
            case 0x06:
                return "Infiniband";
            case 0x07:
                return "IPMI Interface";
            case 0x08:
                return "SERCOS Interface (IEC 61491)";
            case 0x09:
                return "CANbus";
            case 0x80:
                return "SerialBusController - Other";
        }
        return ToHexString(SubclassCode);
    }

    const char* BridgeDeviceSubclassName(uint8_t SubclassCode){
        switch (SubclassCode){
            case 0x00:
                return "Host Bridge";
            case 0x01:
                return "ISA Bridge";
            case 0x02:
                return "EISA Bridge";
            case 0x03:
                return "MCA Bridge";
            case 0x04:
                return "PCI-to-PCI Bridge";
            case 0x05:
                return "PCMCIA Bridge";
            case 0x06:
                return "NuBus Bridge";
            case 0x07:
                return "CardBus Bridge";
            case 0x08:
                return "RACEway Bridge";
            case 0x09:
                return "PCI-to-PCI Bridge";
            case 0x0a:
                return "InfiniBand-to-PCI Host Bridge";
            case 0x80:
                return "Other";
        }
        return ToHexString(SubclassCode);
    }

    const char* GetSubclassName(uint8_t ClassCode, uint8_t SubclassCode){
        switch (ClassCode){
            case 0x01:
                return MassStorageControllerSubclassName(SubclassCode);
            case 0x03:
                switch (SubclassCode){
                    case 0x00:
                        return "VGA Compatible Controller";
                }
            case 0x06:
                return BridgeDeviceSubclassName(SubclassCode);
            case 0x0C:
                return SerialBusControllerSubclassName(SubclassCode);
        }
        return ToHexString(SubclassCode);
    }

    const char* GetProgIFName(uint8_t ClassCode, uint8_t SubclassCode, uint8_t ProgIF){
        switch (ClassCode){
            case 0x01:
                switch (SubclassCode){
                    case 0x06:
                        switch (ProgIF){
                            case 0x00:
                                return "Vendor Specific Interface";
                            case 0x01:
                                return "AHCI 1.0";
                            case 0x02:
                                return "Serial Storage Bus";
                        }
                }
            case 0x03:
                switch (SubclassCode){
                    case 0x00:
                        switch (ProgIF){
                            case 0x00:
                                return "VGA Controller";
                            case 0x01:
                                return "8514-Compatible Controller";
                        }
                }
            case 0x0C:
                switch (SubclassCode){
                    case 0x03:
                        switch (ProgIF){
                            case 0x00:
                                return "UHCI Controller";
                            case 0x10:
                                return "OHCI Controller";
                            case 0x20:
                                return "EHCI (USB2) Controller";
                            case 0x30:
                                return "XHCI (USB3) Controller";
                            case 0x80:
                                return "Unspecified";
                            case 0xFE:
                                return "USB Device (Not a Host Controller)";
                        }
                }    
        }
        return ToHexString(ProgIF);
    }
}