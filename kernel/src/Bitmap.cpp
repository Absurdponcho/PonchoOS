#include "Bitmap.h"

bool Bitmap::operator[](uint64_t index){
    return Get(index);
}

bool Bitmap::Get(uint64_t index){
    if (index > Size * 8) return false;
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t bitIndexer = 0b10000000 >> bitIndex;
    if ((Buffer[byteIndex] & bitIndexer) > 0){
        return true;
    }
    return false;
}

bool Bitmap::Set(uint64_t index, bool value){
    if (index > Size * 8) return false;
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t bitIndexer = 0b10000000 >> bitIndex;
    Buffer[byteIndex] &= ~bitIndexer;
    if (value){
        Buffer[byteIndex] |= bitIndexer;
    }
    return true;
}