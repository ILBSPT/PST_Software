#include <Arduino.h>

#include "HardwareCfg.h"
#include "GlobalVars.h"
#include "StMEvent.h"

uint16_t FP1 = 0, FP2 = 0, FP3 = 0;
uint16_t FL1 = 0, FL2 = 0;

bool IdleCond(void) { return false; }
bool TrueCond(void) { return true; }

bool prog1_finish_cond(void)
{
    if(tank_pressure > FP1)
        return true;
    return false;
}

bool prog2_finish_cond(void)
{
    if(tank_liquid > FL2)
        return true;
    return false;
}

bool prog3_finish_cond(void)
{
    if(tank_pressure > FP3)
        return true;
    return false;
}

bool prog2_stop_cond(void)
{
    if(tank_pressure > FP2 && tank_liquid < FL1)
        return true;
    return false;
}

void prog2_stop_reaction(void)
{
    return;
}

void close_valves(void)
{
    return;
}