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
    return;
}

void read_temperatures(void)
{
    //Dummy function
    return;
}

void V1_close(void)
{

}

void V2_close(void)
{

}

void V3_close(void)
{

}

void V1_open(void)
{

}

void V2_open(void)
{

}

void V3_open(void)
{

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
        command_t command_rep;
        command_rep.cmd = CMD_STATUS_ACK;

        command_rep.size = 2*6 + 1;
        //command_rep.size = 100; //test

        command_rep.data[0] = state;
        command_rep.data[1] = (imu_ax >> 8) & 0xff;
        command_rep.data[2] = (imu_ax) & 0xff ;
        command_rep.data[3] = (imu_ay >> 8) & 0xff;
        command_rep.data[4] = (imu_ay) & 0xff;
        command_rep.data[5] = (imu_az >> 8) & 0xff;
        command_rep.data[6] = (imu_az) & 0xff;
        command_rep.data[7] = (imu_gx >> 8) & 0xff;
        command_rep.data[8] = (imu_gx) & 0xff;
        command_rep.data[9] = (imu_gy >> 8) & 0xff;
        command_rep.data[10] = (imu_gy) & 0xff;
        command_rep.data[11] = (imu_gz >> 8) & 0xff;
        command_rep.data[12] = (imu_gz) & 0xff;
        command_rep.crc = 0x5151;

        write_command(&command_rep, DEFAULT_LOG_INFERFACE);
    }
}