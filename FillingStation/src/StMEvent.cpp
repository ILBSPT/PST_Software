#include <Arduino.h>

#include "HardwareCfg.h"
#include "GlobalVars.h"
#include "StMEvent.h"


bool IdleCond(void) { return false; }
bool TrueCond(void) { return true; }

bool prog1_finish_cond(void)
{
    if(tank_pressure > 1500)
        return true;
    return false;
}

bool prog2_finish_cond(void)
{
    int t = digitalRead(TRIGGER);
    if(tank_pressure > 2500 && !t)
        return true;
    return false;
}

bool prog3_finish_cond(void)
{
    if(tank_pressure > 3500)
        return true;
    return false;
}

bool prog2_stop_cond(void)
{
    int t = digitalRead(TRIGGER);
    if(tank_pressure > 2500 && t)
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