#ifndef HARDWARE_CFG_H_
#define HARDWARE_CFG_H_

#define LED_PIN 2
#define TRIGGER 34 

#define V1_PIN 25
#define V2_PIN 26
#define V3_PIN 27

#define Pressure_PIN 32

#define SERIAL_BAUD 115000
#define SERIAL2_BAUD 115200

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

#define ROCKET_ID 1
#define FILL_STATION_ID 2

#define DEFAULT_ID FILL_STATION_ID

#endif