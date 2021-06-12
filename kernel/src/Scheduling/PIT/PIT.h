#pragma once
#include <stdint.h>

namespace PIT {
    extern double TimeSinceBoot;
    const uint64_t BaseFrequency = 1193182;

    void Sleepd(double Seconds);
    void Sleep(uint64_t MilliSeconds);

    void SetDivisor(uint16_t Divisor);
    uint64_t GetFrequency();
    void SetFrequency(uint64_t Frequency);
    void Tick();
}