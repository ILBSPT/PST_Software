#include <Arduino.h>

#include <time.h>

#include "HardwareCfg.h"
#include "StMWork.h"
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

void blink_led(void)
{
    static int stage = 0;
    if(stage == 0)
        digitalWrite(LED_PIN, 1);
    else if(stage == 1)
        digitalWrite(LED_PIN, 0);
    else if(stage == 2 || stage == 3)
        digitalWrite(LED_PIN, 1);
    else if(stage == 4)
        digitalWrite(LED_PIN, 0);

    stage = (stage + 1) % 5;   
}

void imu_pid_calibration(void)
{
    Serial.println(accelgyro.testConnection() ? "MPu6050 connection successful" : "MPu6050 connection failed");
    Serial.println("PID tuning Each Dot = 100 readings");


  /*A tidbit on how PID (PI actually) tuning works. 
    When we change the offset in the MPU6050 we can get instant results. This allows us to use Proportional and 
    integral of the PID to discover the ideal offsets. Integral is the key to discovering these offsets, Integral 
    uses the error from set-point (set-point is zero), it takes a fraction of this error (error * ki) and adds it 
    to the integral value. Each reading narrows the error down to the desired offset. The greater the error from 
    set-point, the more we adjust the integral value. The proportional does its part by hiding the noise from the 
    integral math. The Derivative is not used because of the noise and because the sensor is stationary. With the 
    noise removed the integral value lands on a solid offset after just 600 readings. At the end of each set of 100 
    readings, the integral value is used for the actual offsets and the last proportional reading is ignored due to 
    the fact it reacts to any noise.
  */
    accelgyro.CalibrateAccel(10);
    accelgyro.CalibrateGyro(10);

    Serial.println("1000 Total Readings");
    accelgyro.PrintActiveOffsets();
    //Serial.println("\n\n Any of the above offsets will work nice \n\n"); 
    //Serial.println("\n\n Any of the above offsets will work nice \n\n Lets proof the PID tuning using another method:"); 
    
    accelgyro.setFullScaleAccelRange(2);

}

void read_IMU(void) 
{
    if(accelgyro.testConnection())
        accelgyro.getMotion6(&imu_ax, &imu_ay, &imu_az,
                             &imu_gx, &imu_gy, &imu_gz);
    return; 
}

void logger(void)
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

void pressure_safety(void)
{

}