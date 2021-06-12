#include "ACPI.h"

namespace ACPI{

    void* FindTable(SDTHeader* SDTHeader, char* Signature){

        int Entries = (SDTHeader->Length - sizeof(ACPI::SDTHeader)) / 8;

        for (int t = 0; t < Entries; t++){
            ACPI::SDTHeader* NewSDTHeader = (ACPI::SDTHeader*)*(uint64_t*)((uint64_t)SDTHeader + sizeof(ACPI::SDTHeader) + (t * 8));
            for (int i = 0; i < 4; i++){
                if (NewSDTHeader->Signature[i] != Signature[i])
                {
                    break;
                }
                if (i == 3) return NewSDTHeader;
            }
        }
        return 0;
    }

}