/**
 * @file main.cpp
 * @author Andre M. (portospaceteam.pt)
 * @brief Entry point for esp32 after boot
 *      Main message parsing and command executing
 *  
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <Arduino.h>

#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "Target.h"
#include "HardwareCfg.h"
#include "GlobalVars.h"

#include "Comms.h"

#include "StateMachine.h"
#include "StMComms.h"
#include "StMWork.h"
#include "FlashLog.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include <I2Cdev.h>
#include <MPU6050.h>
#include <HX711.h>
#include <Max6675.h>
#include <ADS1X15.h>
#include <MCP9600.h>

#include <LoRa.h>

#include <Crc.h>

#include <SerialFlash.h>


// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

MPU6050 accelgyro;
HX711 scale;
ADS1115 ADS(PRESSURE_AMP1_ADDR);
//MAX6675 thermocouple1(SPI_SCLK_PIN, TEMP_AMP1_SS_PIN, SPI_MISO_PIN);
//MAX6675 thermocouple1(4,0,15);
//MAX6675 thermocouple2(SPI_SCLK_PIN, TEMP_AMP2_SS_PIN, SPI_MISO_PIN);
MAX6675 thermocouple1(27, 15, 26);
MAX6675 thermocouple2(27, 0, 26);
MCP9600 thermocouple3;

int led_state = 0;

void echo_reply(command_t* cmd)
{
/*
    Used during testing. See Target.h
*/
#ifdef DIGITAL_TARGET

    Serial.printf("Cmd: %x\n", cmd->cmd);
    Serial.printf("Sz: %x\n", cmd->size);
    if(cmd->size > 0)
    {
        for(int i = 0; i < cmd->size; i++) Serial.printf("%x ", cmd->data[i]);
        Serial.printf("\n");
    }
    Serial.printf("CRC: %x\n", cmd->crc);
#endif
}

void pressure_Setup(void)
{
    Serial.println("Pressure amp starting");
    ADS.begin();
    ADS.setGain(2); //2v
    ADS.setDataRate(7); //fastest
    ADS.setMode(1); //single mode
}

void temp_i2c_Setup(void)
{
    Serial.println("Temp i2c amp starting");
    thermocouple3.begin(0x60);
    thermocouple3.setThermocoupleType(TYPE_K);
    //check if the sensor is connected
    if(thermocouple3.isConnected()){
        Serial.println("Device will acknowledge!");
    }
    else {
        Serial.println("Device did not acknowledge! Freezing.");
        while(1); //hang forever
    }
}

void loadCell_Setup(void)
{
    Serial.println("Load CELL starting");
    scale.begin(LOADCELL_OUT_PIN, LOADCELL_SCK_PIN);
}

void gyroSetup(void)
{
    Serial.println("Gyro starting");
    accelgyro.initialize();
    Wire.setBufferSize(256);
 // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPu6050 connection successful" : "MPu6050 connection failed");
    
}

void Valves_Setup(void)
{
    Serial.println("Valves starting");
    pinMode(Vpu_PIN, OUTPUT);
    //pinMode(V4_PIN, OUTPUT);

    //pinMode(Pressure_PIN, INPUT);
}

void Flash_Setup()
{
    if (!SD.begin(Flash_SS_PIN)) {
        printf("Unable to access SPI Flash chip\n");
        return;
    }

    current_id = get_last_id() + 1;
    Serial.print("current id");
    Serial.print(current_id);
    Serial.printf("\n");

}

void LoRa_Setup(void)
{
  LoRa.setPins(LORA_SS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);
  //LoRa.setSignalBandwidth(500E3);
  //LoRa.setCodingRate4(5);
  //LoRa.setSpreadingFactor(7);
  //LoRa.setGain(1);

  Serial.println("Lora starting");
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    //while (1);
  }
}

void setup() {

    Serial.begin(SERIAL_BAUD); //USBC serial

    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    
    //gyroSetup();
    LoRa_Setup();

    Valves_Setup();

    loadCell_Setup();

    pressure_Setup();

    //temp_i2c_Setup();

    Flash_Setup();

    //setup trigger switch
    pinMode(TRIGGER, INPUT_PULLUP);

    //Set board LED 
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    char arr[] = {'a','b','c','d'};
    //unsigned long crc_1 = crc((unsigned char*)arr, 4); 

/*
 Removed during testing. See Target.h
 */
#ifndef DIGITAL_TARGET

    Serial2.begin(SERIAL2_BAUD);
#endif

    printf("Setup done\n");
}

void loop() {
    rocket_state_t command_state = state, \
                   event_state   = state; 

    /*
        Execute the state function 
     */
    bool work_performed = WORK_HANDLER();

    /*
     Event handling
     */
    if (work_performed) event_state = EVENT_HANDLER();
    if(event_state == -1) event_state = state;

    /*
     Comms
     */
    int error;

    //check if we have new data
    command_t* cmd = read_command(&error, DEFAULT_CMD_INTERFACE);
    if( cmd != NULL && 
        error == CMD_READ_OK && 
        run_command(cmd, state, DEFAULT_CMD_INTERFACE) == CMD_RUN_OK) 
    {
        //make transition to new state on the state machine
        if(state_machine[state].comms[cmd->cmd] != -1)
            //we have new state, use lookup table
            command_state = (rocket_state_t)comm_transition[state][cmd->cmd];
    }

    /*
     * Do state transition
     */
    if(command_state != state) 
        //command change of state as priority over
        //internal events changes of state
        state = command_state;
    else if(event_state != state)
        //only if comms haven't changed the state we can
        //acept a new state from internal events
        state = event_state;


    //delay(1);
}
