#ifndef HARDWARE_CFG_H_
#define HARDWARE_CFG_H_

#define LED_PIN 2
#define TRIGGER 34 

#define V1_PIN 13
#define V2_PIN 13
#define V3_PIN 13

#define Pressure_PIN 32

//#define Flash_SS_PIN 5
#define Flash_SS_PIN 25
#define LORA_SS_PIN 5
//#define TEMP_AMP1_SS_PIN 26
#define TEMP_AMP1_SS_PIN 0
//#define TEMP_AMP2_SS_PIN 27
#define TEMP_AMP2_SS_PIN 26

#define LORA_RESET_PIN 14
#define LORA_DIO0_PIN 36

#define LOADCELL_OUT_PIN 32 
#define LOADCELL_SCK_PIN 33
#define SERIAL_BAUD 115000
//#define SERIAL2_BAUD 115200
#define SERIAL2_BAUD 57600

#define PRESSURE_AMP1_ADDR 0x48

typedef enum
{
    LoRa_INTERFACE,
    RS485_INTERFACE,
    Uart_INTERFACE,
    interface_t_size
} interface_t;

//#define DEFAULT_CMD_INTERFACE Uart_INTERFACE 
#define DEFAULT_CMD_INTERFACE Uart_INTERFACE
#define DEFAULT_LOG_INFERFACE RS485_INTERFACE
#define DEFAULT_SYNC_INTERFACE Uart_INTERFACE

#define GROUND_ID 0
#define ROCKET_ID 1
#define FILL_STATION_ID 2

#define DEFAULT_ID FILL_STATION_ID

#endif