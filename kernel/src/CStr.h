#pragma once
#include <stdint.h>

const char* ToString(uint64_t value);
const char* ToString(int64_t value);
const char* ToHexString(uint64_t value);
const char* ToHexString(uint32_t value);
const char* ToHexString(uint16_t value);
const char* ToHexString(uint8_t value);
const char* ToString(double value, uint8_t decimalPlaces);
const char* ToString(double value);
