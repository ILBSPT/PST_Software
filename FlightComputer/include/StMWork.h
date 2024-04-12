#ifndef STM_WORK_H_
#define STM_WORK_H_


#include "StateMachine.h"

/*
 * Define here all functions that can run as work in a state
 */

void idle_state(void);
void toggle_led_high(void);
void toggle_led_low(void);
void toggle_led(void);
void blink_led(void);

void read_pressures(void);
void read_temperatures(void);
void calc_liquid(void);

void Vpu_close(void);
void V4_close(void);

void Vpu_open(void);
void V4_open(void);

void imu_pid_calibration(void);
void read_IMU(void); 

void logger(void);
#endif