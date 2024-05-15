#include <Arduino.h>

#include <time.h>

#include "HardwareCfg.h"
#include "StMWork.h"
#include "StateMachine.h"
#include "GlobalVars.h"
#include "Comms.h"

#include <I2Cdev.h>
#include <MPU6050.h>

int16_t imu_ax;
int16_t imu_ay;
int16_t imu_az;

int16_t imu_gx;
int16_t imu_gy;
int16_t imu_gz;

uint16_t tank_pressure = 0;
uint16_t tank_liquid = 0;

void idle_state(void) { return; }

void toggle_led_high(void)
{
    digitalWrite(LED_PIN, HIGH);
}

void toggle_led(void)
{
    static int led_status = 0;
    led_status ^= 1;
    digitalWrite(LED_PIN, led_status);
}

void read_pressures(void)
{
    //Dummy function
    //read pressure of the fedding lines
    return;
}

void read_temperatures(void)
{
    //Dummy function
    return;
}

void V1_close(void)
{
    digitalWrite(V1_PIN, 0);
}

void V2_close(void)
{
    digitalWrite(V2_PIN, 0);
}

void V3_close(void)
{
    digitalWrite(V3_PIN, 0);
}

void V1_open(void)
{
    digitalWrite(V1_PIN, 1);
}

void V2_open(void)
{
    digitalWrite(V2_PIN, 1);
}

void V3_open(void)
{
    digitalWrite(V3_PIN, 1);
}

void logger(void)
{

}

void echo_reply(void)
{
    int error;
    command_t* cmd = read_command(&error, DEFAULT_LOG_INFERFACE);
    if( cmd != NULL &&
        error == CMD_READ_OK &&
        cmd->cmd == CMD_STATUS) 
    {
        /* Update rocket tank values */
        tank_pressure =  (cmd->data[1] << 8) + (cmd->data[2]);
        tank_liquid =  (cmd->data[3] << 8) + (cmd->data[4]);

        /* Send response to the bus*/
        command_t command_rep;
        command_rep.cmd = CMD_STATUS_ACK;
        command_rep.id = DEFAULT_ID;

        command_rep.size = 2*4 + 1;
        //command_rep.size = 100; //test

        command_rep.data[0] = state;
        command_rep.data[1] = 0;
        command_rep.data[2] = 1;
        command_rep.data[3] = 2;
        command_rep.data[4] = 3;
        command_rep.data[5] = 4;
        command_rep.data[6] = 5;
        command_rep.data[7] = 6;
        command_rep.data[8] = 7;
        command_rep.crc = 0x5252;

        write_command(&command_rep, DEFAULT_LOG_INFERFACE);
    }
}