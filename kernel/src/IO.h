#pragma once
#include <stdint.h>

void OutByte (uint16_t Port, uint8_t Value);
uint8_t InByte(uint16_t Port);
void IOWait();