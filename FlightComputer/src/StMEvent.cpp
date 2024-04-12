#include <Arduino.h>

#include "HardwareCfg.h"
#include "GlobalVars.h"
#include "StMEvent.h"

uint16_t RP1 = 0, RP2 = 0, RP3 = 0;

bool IdleCond(void) { return false; }
bool TrueCond(void) { return true; }

bool prog2_finish_cond(void)
{
    if(tank_pressure > RP3)
        return true;
    return false;
}

bool prog1_stop_cond(void)
{
    if(tank_pressure > RP1)
        return true;
    return false;
}

bool safety_stop_cond(void)
{
    if(tank_pressure < RP2)
        return true;
    return false;
}

bool IgniteCond(void)
{
    if(digitalRead(TRIGGER) == LOW)
        return true;
    return false;
}

void IgniteReaction()
{
    return;
}