#include <Arduino.h>

#include <time.h>

#include "StateMachine.h"
#include "StMWork.h"
#include "StMEvent.h"

#include <stdarg.h>
#include <limits.h>

rocket_state_t state = IDLE; 

//need to add a stop and see how the executin function changes
rocket_state_t comm_transition[rocket_state_size][cmd_size] = {  
//                STATUS ABORT EXEC  STOP   FUELING  READY  ARM   LED_ON LED_OFF IMU_CALIB  RESUME    [LOG]    
/* Idle     */  {   -1,  ABORT, -1,   -1,   FUELING, READY, -1,     -1,     -1,  IMU_TUNE,    -1,  -1,-1,-1,-1},
/* Fueling  */  {   -1,  IDLE,  -1,   IDLE,   -1,     -1,   -1,     -1,     -1,     -1,       -1,  -1,-1,-1,-1},
/* Prog 1   */  {   -1,  ABORT, -1, FUELING,  -1,     -1,   -1,     -1,     -1,     -1,       -1,  -1,-1,-1,-1},
/* Prog 2   */  {   -1,  ABORT, -1, FUELING,  -1,     -1,   -1,     -1,     -1,     -1,       -1,  -1,-1,-1,-1},
/* Safety   */  {   -1,  ABORT, -1, FUELING,  -1,     -1,   -1,     -1,     -1,     -1,       -1,  -1,-1,-1,-1},
/* Ready    */  {   -1,  IDLE,  -1,   IDLE,   -1,     -1,   ARMED,  -1,     -1,  IMU_TUNE,    -1,  -1,-1,-1,-1},
/* Armed    */  {   -1,  IDLE,  -1,  READY,   -1,     -1,   -1,     -1,     -1,  IMU_TUNE,    -1,  -1,-1,-1,-1},
/* Launch   */  {   -1,  ABORT, -1,   -1,     -1,     -1,   -1,     -1,     -1,  IMU_TUNE,    -1,  -1,-1,-1,-1},
/* Abort    */  {   -1,   -1,   -1,   -1,     -1,    IDLE,  -1,     -1,     -1,  IMU_TUNE,    -1,  -1,-1,-1,-1},
/* IMU PID  */  {   -1,  IDLE,  -1,   -1,     -1,     -1,   -1,     -1,     -1,  IMU_TUNE,    -1,  -1,-1,-1,-1}
};

/*1*/ // don't allow sensor calibration once rocket is armed, arming the rocket hould be the last step before laucnh (right?)
/*2*/ //once rocket is sent to abort, the full start up process needs to be redone 
// (maybe imu initialization needs to be its own single work fnc state like imu calibration)  thru imu initialization swf mayge go to imu calibration swf

State_t state_machine[rocket_state_size] = 
{
    //IDLE
    {
        .work = { {.chanel = read_IMU, .delay = 1000, .begin = 0},
                  {.chanel = toggle_led, .delay = 1000, .begin = 0} },

        .events = {},

        .comms = comm_transition[IDLE],
    },
    //FUELING
    {
        .work = { {.chanel = read_pressures, .delay = 10, .begin = 0},
                  {.chanel = read_temperatures, .delay = 10, .begin = 0},
                  {.chanel = calc_liquid, .delay = 10, .begin = 0},
                  {.chanel = blink_led, .delay = 200, .begin = 0},
                  {.chanel = logger, .delay = 20, .begin = 0} },

        .events = {},

        .comms = comm_transition[FUELING],
    },
    //PROG1
    {
        .work = { {.chanel = read_pressures, .delay = 10, .begin = 0},
                  {.chanel = read_temperatures, .delay = 10, .begin = 0},
                  {.chanel = calc_liquid, .delay = 10, .begin = 0},
                  {.chanel = Vpu_close, .delay = 500, .begin = 0},
                  {.chanel = blink_led, .delay = 200, .begin = 0},
                  {.chanel = logger, .delay = 20, .begin = 0} },

        .events = { {.condition = prog1_stop_cond, .reaction = Vpu_open, .next_state = SAFETY} },

        .comms = comm_transition[PROG1],
    },
    //PROG2
    {
        .work = { {.chanel = read_pressures, .delay = 10, .begin = 0},
                  {.chanel = read_temperatures, .delay = 10, .begin = 0},
                  {.chanel = calc_liquid, .delay = 10, .begin = 0},
                  {.chanel = Vpu_open, .delay = 500, .begin = 0},
                  {.chanel = blink_led, .delay = 200, .begin = 0},
                  {.chanel = logger, .delay = 20, .begin = 0} },

        .events = { {.condition = prog2_finish_cond, .reaction = Vpu_close, .next_state = FUELING} },

        .comms = comm_transition[PROG2],
    },
    //SAFETY
    {
        .work = { {.chanel = read_pressures, .delay = 10, .begin = 0},
                  {.chanel = read_temperatures, .delay = 10, .begin = 0},
                  {.chanel = calc_liquid, .delay = 10, .begin = 0},
                  {.chanel = Vpu_open, .delay = 500, .begin = 0},
                  {.chanel = blink_led, .delay = 200, .begin = 0},
                  {.chanel = logger, .delay = 20, .begin = 0} },

        .events = { {.condition = safety_stop_cond, .reaction = Vpu_close, .next_state = PROG1} },

        .comms = comm_transition[SAFETY],
    },
    //READY
    {
        .work = { {.chanel = read_IMU, .delay = 10, .begin = 0},
                  {.chanel = toggle_led, .delay = 500, .begin = 0},
                  {.chanel = logger, .delay = 20, .begin = 0} },

        .events = {},

        .comms = comm_transition[READY],

    },
    //ARMED
    {
        .work = { {.chanel = read_IMU, .delay = 10, .begin = 0},
                  {.chanel = toggle_led, .delay = 200, .begin = 0} },

        .events = { {IgniteCond, IgniteReaction, LAUNCH} },

        .comms = comm_transition[ARMED],

    },
    //LAUNCH
    {
        .work = { {.chanel = read_IMU, .delay = 10, .begin = 0},
                  {.chanel = toggle_led_high, .delay = 1000, .begin = 0} },

        .events = {},

        .comms = comm_transition[LAUNCH],

    },
    //ABORT
    {
        .work = { {.chanel = read_IMU, .delay = 10} },

        .events = {},

        .comms = comm_transition[ABORT],

    },
    //IMU_TUNE
    {
        .work = { {.chanel = imu_pid_calibration, .delay = 1000, .begin = 0}, },

        .events = { {TrueCond, NULL, IDLE} }, //this will always evaluate true, imu calib is run one time and is automaticly sent to idle

        .comms = comm_transition[IMU_TUNE],
    }
};

rocket_state_t event_handler()
{
    for(int i = 0; i < MAX_EVENT_SIZE; i++)
    {
        bool (*cond)(void) = state_machine[state].events[i].condition;
        void (*react)(void) = state_machine[state].events[i].reaction;
        int next_state = state_machine[state].events[i].next_state;

        if(cond == NULL)
            continue;

        if(cond())
        {
            if(react != NULL) 
                react();
            if(next_state > -1)
                return next_state;
        }
    }

    return -1;
}

bool exec()
{
    //printf("exec size %d work %x\n", size, work);
    unsigned long end = millis();
    bool change = false;
    for(int i = 0; i < MAX_WORK_SIZE; i++)
    {

        if(state_machine[state].work[i].chanel == NULL)
            continue;

        int msec = (end - state_machine[state].work[i].begin);
        if(msec > state_machine[state].work[i].delay)
        {
            state_machine[state].work[i].begin = end;
            state_machine[state].work[i].chanel(); //execute sample function
            change = true;
        }
    }

    return change;
}