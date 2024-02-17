#include <Arduino.h>

#include <time.h>

#include "StateMachine.h"
#include "StMWork.h"
#include "StMEvent.h"

#include <stdarg.h>
#include <limits.h>


//need to add a stop and see how the executin function changes
rocket_state_t comm_transition[rocket_state_size][cmd_size] = {  
//               Status    Ready    Arm     Abort   LED_ON    LED_OFF  IMU_CALIBRATE  
/* Idle      */   {-1,     READY,    -1,    ABORT,    -1,        -1,   IMU_PID_TUNE },      
/* Ready     */   {-1,      -1,     ARMED,   IDLE,    -1,        -1,   IMU_PID_TUNE },
/* Armed     */   {-1,      -1,      -1,     IDLE,    -1,        -1,   IMU_PID_TUNE },
/* Launch    */   {-1,      -1,      -1,    ABORT,    -1,        -1,        -1 /*1*/},
/* Abort     */   {-1, /*2*/IDLE,    -1,      -1,     -1,        -1,        -1      },
/* IMU PID   */   {-1,      -1,      -1,     IDLE,    -1,        -1,        -1      }
};

/*1*/ // don't allow sensor calibration once rocket is armed, arming the rocket hould be the last step before laucnh (right?)
/*2*/ //once rocket is sent to abort, the full start up process needs to be redone 
// (maybe imu initialization needs to be its own single work fnc state like imu calibration)  thru imu initialization swf mayge go to imu calibration swf

State_t state_machine[rocket_state_size] = 
{
    //IDLE
    {
        .work = { {.chanel = read_IMU, .delay = 10, .begin = 0},
                  {.chanel = toggle_led, .delay = 1000, .begin = 0} },

        .events = {},

        .comms = comm_transition[IDLE],
    },
    //READY
    {
        .work = { {.chanel = read_IMU, .delay = 10, .begin = 0},
                  {.chanel = toggle_led, .delay = 500, .begin = 0} },

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
        .work = { {.chanel = read_IMU, .delay = 10},
                  {.chanel = toggle_led_high, .delay = 1000} },

        .events = {},

        .comms = comm_transition[ABORT],

    },
    //IMU_PID_TUNE
    {
        .work = { {.chanel = imu_pid_calibration, .delay = 1000, .begin = 0}, },

        .events = { {TrueCond, NULL, IDLE} }, //this will always evaluate true, imu calib is run one time and is automaticly sent to idle

        .comms = comm_transition[IMU_PID_TUNE],
    }
};

rocket_state_t event_handler(State_t * states, rocket_state_t state)
{
    for(int i = 0; i < MAX_EVENT_SIZE; i++)
    {
        bool (*cond)(void) = states[state].events[i].condition;
        void (*react)(void) = states[state].events[i].reaction;
        int next_state = states[state].events[i].next_state;

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

bool exec(State_t * states, rocket_state_t state)
{
    //printf("exec size %d work %x\n", size, work);
    unsigned long end = millis();
    bool change = false;
    for(int i = 0; i < MAX_WORK_SIZE; i++)
    {

        if(states[state].work[i].chanel == NULL)
            continue;

        int msec = (end - states[state].work[i].begin);
        if(msec > states[state].work[i].delay)
        {
            states[state].work[i].begin = end;
            states[state].work[i].chanel(); //execute sample function
            change = true;
        }
    }

    return change;
}