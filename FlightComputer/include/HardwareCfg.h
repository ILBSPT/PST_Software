#ifndef HARDWARE_CFG_H_
#define HARDWARE_CFG_H_

#define LED_PIN 2
#define TRIGGER 34 

#define Vpu_PIN 25
#define V4_PIN 26

#define Pressure_PIN 15
#define TankL_PIN 0

#define Flash_SS_PIN 10
#define LoRa_SS_PIN 11

#define SERIAL_BAUD 115000
#define SERIAL2_BAUD 115200

typedef enum
{
    LoRa_INTERFACE,
    RS485_INTERFACE,
    Uart_INTERFACE,
    interface_t_size
} interface_t;

#define DEFAULT_CMD_INTERFACE Uart_INTERFACE 
#define DEFAULT_LOG_INFERFACE RS485_INTERFACE
#define DEFAULT_SYNC_INTERFACE Uart_INTERFACE

#endif