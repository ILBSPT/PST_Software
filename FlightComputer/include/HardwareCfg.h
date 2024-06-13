#ifndef HARDWARE_CFG_H_
#define HARDWARE_CFG_H_

#define SPI_SCLK_PIN 18
#define SPI_MISO_PIN 19
#define SPI_MOSI_PIN 23

#define LED_PIN 2
#define TRIGGER 34 

#define Vpu_PIN 13
#define V4_PIN 13

//#define Pressure_PIN 15
//#define TankL_PIN 0

//#define Flash_SS_PIN 5
#define Flash_SS_PIN 25
#define LORA_SS_PIN 5
#define TEMP_AMP1_SS_PIN 4
#define TEMP_AMP2_SS_PIN 0

#define LORA_RESET_PIN 14
#define LORA_DIO0_PIN 36

#define LOADCELL_OUT_PIN 32 
#define LOADCELL_SCK_PIN 33

#define SERIAL_BAUD 115000
//#define SERIAL2_BAUD 115200
#define SERIAL2_BAUD 57600 

#define PRESSURE_AMP1_ADDR 0x48

#endif