#ifndef HARDWARE_CFG_H_
#define HARDWARE_CFG_H_

#define LED_PIN 2
#define TRIGGER 34 

#define SERIAL_BAUD 115000
#define SERIAL2_BAUD 115200

typedef enum
{
    LoRa_INTERFACE,
    RS485_INTERFACE,
    Uart_INTERFACE,
    interface_t_size
} interface_t;

#define DEFAULT_CMD_INTERFACE LoRa_INTERFACE
#define DEFAULT_LOG_INFERFACE Uart_INTERFACE
#define DEFAULT_SYNC_INTERFACE Uart_INTERFACE

#endif