#include "PIT.h"
#include "../../IO.h"

namespace PIT{
    double TimeSinceBoot = 0;

    uint16_t Divisor = 65535;

    void Sleepd(double seconds){
        double startTime = TimeSinceBoot;
        while (TimeSinceBoot < startTime + seconds){
            asm("hlt");
        }
    }

    void Sleep(uint64_t milliseconds){
        Sleepd((double)milliseconds / 1000);
    }

    void SetDivisor(uint16_t Divisor){
        if (Divisor < 100) Divisor = 100;
        Divisor = Divisor;
        OutByte(0x40, (uint8_t)(Divisor & 0x00ff));
        IOWait();
        OutByte(0x40, (uint8_t)((Divisor & 0xff00) >> 8));
    }

    uint64_t GetFrequency(){
        return BaseFrequency / Divisor;
    }

    void SetFrequency(uint64_t frequency){
        SetDivisor(BaseFrequency / frequency);
    }

    void Tick(){
        TimeSinceBoot += 1 / (double)GetFrequency();
    }
}