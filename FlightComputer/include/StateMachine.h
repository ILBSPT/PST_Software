#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include "Comms.h"

#define MAX_WORK_SIZE 10
#define MAX_EVENT_SIZE 10

//use the enum below as the values of rocket_state_t
//use -1 for default behavior
typedef int8_t rocket_state_t;
enum 
{
    IDLE,
    
    FUELING,
    PROG1,
    PROG2,
    SAFETY,

    READY,
    ARMED,
    LAUNCH,

    ABORT,

    IMU_TUNE,

    rocket_state_size, //this needs to be the last state for size to work
};

typedef struct 
{
    bool (*condition)(void);
    void (*reaction)(void);
    rocket_state_t next_state;
} Event_t;

typedef struct
{
    void (*chanel)(void);
    unsigned long delay; //millis of delay between samples
    unsigned long begin;
} Work_t;

typedef struct
{
    Work_t work[MAX_WORK_SIZE];
    Event_t events[MAX_EVENT_SIZE];
    rocket_state_t *comms;

    //used as the time base when dealing with sensor sampling rate and delays
    unsigned long entry_time; 
} State_t;

extern rocket_state_t state; 
extern rocket_state_t comm_transition[rocket_state_size][cmd_size]; //save transition state for communication
extern State_t state_machine[rocket_state_size]; 


rocket_state_t event_handler();
bool work_handler();
#define WORK_HANDLER() work_handler()
#define EVENT_HANDLER() event_handler()


#endif