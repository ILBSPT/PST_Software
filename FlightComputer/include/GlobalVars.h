#ifndef _STM_VAR_H_
#define _STM_VAR_H_

#include <inttypes.h>

#include <I2Cdev.h>
#include <MPU6050.h>
#include <HX711.h>

//----------- IMU vars ------------
extern MPU6050 accelgyro;

//-----------LOADCELL--------------
extern HX711 scale;

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

//---------------Tank sensor state--------------
extern uint16_t tank_pressure;
extern uint16_t tank_liquid;


//---------------Tank stm vars----------------
extern uint16_t RP1, RP2, RP3;

#endif