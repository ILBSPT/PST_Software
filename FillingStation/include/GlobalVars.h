#ifndef _STM_VAR_H_
#define _STM_VAR_H_

#include <inttypes.h>

#include <I2Cdev.h>
#include <MPU6050.h>

//----------- IMU vars ------------
extern MPU6050 accelgyro;

//lattest readding s from the imu sensor
extern int16_t imu_ax;
extern int16_t imu_ay;
extern int16_t imu_az;

extern int16_t imu_gx;
extern int16_t imu_gy;
extern int16_t imu_gz;

//---------------Valve state------------
//extern uint8_t v1;
//extern uint8_t v3;
//extern uint8_t v2;


//---------------Tank sensor state--------------
extern uint16_t tank_pressure;
extern uint16_t tank_liquid;


//---------------Tank stm vars----------------
extern uint16_t FP1, FP2, FP3;
extern uint16_t FL1, FL2;

#endif