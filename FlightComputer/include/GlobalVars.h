#ifndef _STM_VAR_H_
#define _STM_VAR_H_

#include <inttypes.h>

#include <I2Cdev.h>
#include <MPU6050.h>
#include <HX711.h>
#include <Max6675.h>
#include <ADS1X15.h>


//----------- IMU vars ------------
extern MPU6050 accelgyro;

extern int16_t imu_ax;
extern int16_t imu_ay;
extern int16_t imu_az;

extern int16_t imu_gx;
extern int16_t imu_gy;
extern int16_t imu_gz;


//-----------LOADCELL--------------
extern HX711 scale;

extern int32_t weight;

//-----------Pressure AMP--------------
extern ADS1115 ADS;

extern int16_t tank_pressure1;
extern int16_t tank_pressure2;

//-----------Temperature AMP--------------
extern MAX6675 thermocouple1;
extern MAX6675 thermocouple2;

extern int16_t tank_temp1;
extern int16_t tank_temp2;

//-----------Temperature AMP I2C--------------
extern MCP9600 thermocouple3;
//for now it uses the tank_temp2

//---------------Valve state------------
//extern uint8_t v1;
//extern uint8_t v3;

//---------------Tank state--------------

extern uint16_t tank_liquid;
extern uint16_t tank_pressure; //testing only

//---------------Tank CMD vars vars----------------
extern uint16_t RP1, RP2, RP3;

#endif