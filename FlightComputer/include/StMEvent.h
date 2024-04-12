#ifndef STM_EVENT_H_
#define STM_EVENT_H_

#include "StateMachine.h"

/*
 * Define here all conditions for events and reaction functions 
 */


//rocket_state_t event_handler(rocket_state_t state);
bool IdleCond(void);
bool TrueCond(void);


bool prog2_finish_cond(void);
bool prog1_stop_cond(void);
bool safety_stop_cond(void);

bool IgniteCond(void);
void IgniteReaction();

#endif