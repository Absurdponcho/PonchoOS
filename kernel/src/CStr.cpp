#include "CStr.h"

char UIntToStringOutput[128];
const char* ToString(uint64_t Value){
    uint8_t Size;
    uint64_t SizeTest = Value;
    while (SizeTest / 10 > 0){
        SizeTest /= 10;
        Size++;
    }

    uint8_t Index = 0;
    while(Value / 10 > 0){
        uint8_t Remainder = Value % 10;
        Value /= 10;
        UIntToStringOutput[Size - Index] = Remainder + '0';
        Index++;
    }
    uint8_t Remainder = Value % 10;
    UIntToStringOutput[Size - Index] = Remainder + '0';
    UIntToStringOutput[Size + 1] = 0; 
    return UIntToStringOutput;
}

char HexToStringOutput[128];
const char* ToHexString(uint64_t Value){
    uint64_t* ValuePtr = &Value;
    uint8_t* Ptr;
    uint8_t Temp;
    uint8_t Size = 8 * 2 - 1;
    for (uint8_t i = 0; i < Size; i++){
        Ptr = ((uint8_t*)ValuePtr + i);
        Temp = ((*Ptr & 0xF0) >> 4);
        HexToStringOutput[Size - (i * 2 + 1)] = Temp + (Temp > 9 ? 55 : '0');
        Temp = ((*Ptr & 0x0F));
        HexToStringOutput[Size - (i * 2)] = Temp + (Temp > 9 ? 55 : '0');
    }
    HexToStringOutput[Size + 1] = 0;
    return HexToStringOutput;
}

char HexToStringOutput32[128];
const char* ToHexString(uint32_t Value){
    uint32_t* ValuePtr = &Value;
    uint8_t* Ptr;
    uint8_t Temp;
    uint8_t Size = 4 * 2 - 1;
    for (uint8_t i = 0; i < Size; i++){
        Ptr = ((uint8_t*)ValuePtr + i);
        Temp = ((*Ptr & 0xF0) >> 4);
        HexToStringOutput32[Size - (i * 2 + 1)] = Temp + (Temp > 9 ? 55 : '0');
        Temp = ((*Ptr & 0x0F));
        HexToStringOutput32[Size - (i * 2)] = Temp + (Temp > 9 ? 55 : '0');
    }
    HexToStringOutput32[Size + 1] = 0;
    return HexToStringOutput32;
}

char HexToStringOutput16[128];
const char* ToHexString(uint16_t Value){
    uint16_t* ValuePtr = &Value;
    uint8_t* Ptr;
    uint8_t Temp;
    uint8_t Size = 2 * 2 - 1;
    for (uint8_t i = 0; i < Size; i++){
        Ptr = ((uint8_t*)ValuePtr + i);
        Temp = ((*Ptr & 0xF0) >> 4);
        HexToStringOutput16[Size - (i * 2 + 1)] = Temp + (Temp > 9 ? 55 : '0');
        Temp = ((*Ptr & 0x0F));
        HexToStringOutput16[Size - (i * 2)] = Temp + (Temp > 9 ? 55 : '0');
    }
    HexToStringOutput16[Size + 1] = 0;
    return HexToStringOutput16;
}

char HexToStringOutput8[128];
const char* ToHexString(uint8_t Value){
    uint8_t* ValuePtr = &Value;
    uint8_t* Ptr;
    uint8_t Temp;
    uint8_t Size = 1 * 2 - 1;
    for (uint8_t i = 0; i < Size; i++){
        Ptr = ((uint8_t*)ValuePtr + i);
        Temp = ((*Ptr & 0xF0) >> 4);
        HexToStringOutput8[Size - (i * 2 + 1)] = Temp + (Temp > 9 ? 55 : '0');
        Temp = ((*Ptr & 0x0F));
        HexToStringOutput8[Size - (i * 2)] = Temp + (Temp > 9 ? 55 : '0');
    }
    HexToStringOutput8[Size + 1] = 0;
    return HexToStringOutput8;
}

char IntToStringOutput[128];
const char* ToString(int64_t Value){
    uint8_t IsNegative = 0;

    if (Value < 0){
        IsNegative = 1;
        Value *= -1;
        IntToStringOutput[0] = '-';
    }

    uint8_t Size;
    uint64_t SizeTest = Value;
    while (SizeTest / 10 > 0){
        SizeTest /= 10;
        Size++;
    }

    uint8_t Index = 0;
    while(Value / 10 > 0){
        uint8_t Remainder = Value % 10;
        Value /= 10;
        IntToStringOutput[IsNegative + Size - Index] = Remainder + '0';
        Index++;
    }
    uint8_t Remainder = Value % 10;
    IntToStringOutput[IsNegative + Size - Index] = Remainder + '0';
    IntToStringOutput[IsNegative + Size + 1] = 0; 
    return IntToStringOutput;
}

char DoubleToStringOutput[128];
const char* ToString(double Value, uint8_t DecimalPlaces){
    if (DecimalPlaces > 20) DecimalPlaces = 20;

    char* IntPtr = (char*)ToString((int64_t)Value);
    char* DoublePtr = DoubleToStringOutput;

    if (Value < 0){
        Value *= -1;
    }

    while(*IntPtr != 0){
        *DoublePtr = *IntPtr;
        IntPtr++;
        DoublePtr++;
    }

    *DoublePtr = '.';
    DoublePtr++;

    double NewValue = Value - (int)Value;

    for (uint8_t i = 0; i < DecimalPlaces; i++){
        NewValue *= 10;
        *DoublePtr = (int)NewValue + '0';
        NewValue -= (int)NewValue;
        DoublePtr++;
    }

    *DoublePtr = 0;
    return DoubleToStringOutput;
}

const char* ToString(double Value){
    return ToString(Value, 2);
}
