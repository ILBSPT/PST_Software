#include <Arduino.h>

#include <inttypes.h>
#include <time.h>
#include <string.h>

#include "Target.h"
#include "Comms.h"

#include "FlashLog.h"

#include <LoRa.h>

void write_command(command_t* cmd, interface_t interface)
{
    int size = 0;
    uint8_t buff[MAX_COMMAND_BUFFER + 5] = {0};
    
    buff[size++] = 0x55;
    buff[size++] = cmd->cmd;
    buff[size++] = cmd->id;
    buff[size++] = cmd->size;
    for(int i = 0; i < cmd->size; i++)
        buff[size++] = cmd->data[i];
    buff[size++] = ((cmd->crc >> 8) & 0xff);
    buff[size++] = ((cmd->crc) & 0xff);

    log_command(cmd);
    
    switch(interface)
    {
        case LoRa_INTERFACE:
        {
            LoRa.beginPacket();
            int sz = LoRa.write(buff, size);
            LoRa.endPacket(true);
            //Serial.printf("Lora sent %d packets\n");
        }
        break;
        case RS485_INTERFACE:
            Serial2.write(buff, size);
        break;
        case Uart_INTERFACE:
            Serial.write(buff, size);
        break;
        default:
        break;
    }
}

static COMMAND_STATE parse_input(uint8_t read_byte, command_t* command, COMMAND_STATE cmd_state)
{
    uint8_t state = cmd_state;

    //printf("data %x\n", read_byte);
    switch(state)
    {
        case SYNC:
            if(read_byte == 0x55)
            {
                //start timeout timer
                state = CMD;
                command->data_recv = 0;
                memset(command, 0, sizeof(command_t));
                command->begin = clock();
            }
        break;

        case CMD:
            command->cmd = (cmd_type_t)read_byte;
            state = ID;
        break;

        case ID:
            command->id = (cmd_type_t)read_byte;
            state = SIZE;
        break;

        case SIZE:                
            command->size = read_byte;
            if(command->size == 0)
                state = CRC1;
            else state = DATA;
        break;

        case DATA:
            command->data[command->data_recv++] = read_byte;
            if(command->data_recv == command->size)
                state = CRC1;
        break;

        case CRC1:
            command->crc = read_byte << 8;
            state = CRC2;
        break;

        case CRC2:
            command->crc += read_byte;
            state = END;
        break;

        default:
            state = SYNC;
    };

    return (COMMAND_STATE)state;
}

command_t* read_command(int* error, interface_t interface)
{
    static command_t command_arr[interface_t_size];
    static COMMAND_STATE state_arr[interface_t_size] = {SYNC};
    static clock_t end_arr[interface_t_size] = {0};
    
    uint8_t index = (uint8_t)interface;
    command_t *command = &command_arr[index];
    COMMAND_STATE *state = &state_arr[index];
    clock_t *end = &end_arr[index];

    size_t size;
    uint8_t read_byte;

    switch(interface)
    {
        case LoRa_INTERFACE:
        {
            //Work arourd for lora to not spam the spi bus
            static unsigned int begin = 0, end = 0;
            end = millis();
            if(end - begin < 1) break;
            begin = end;
            
            int packetSize = LoRa.parsePacket();
            //if(packetSize > 0) Serial.printf("packet recived %d\n", packetSize);
            while(packetSize != 0 && LoRa.available() && *state != END)
            {
                read_byte = LoRa.read();
                *state = parse_input(read_byte, command, *state);
            }
        }
        break;

        case RS485_INTERFACE:
        {
            while(Serial2.available() && *state != END)
            {
                read_byte = Serial2.read();
                *state = parse_input(read_byte, command, *state);
            }
        }
        break;
        
        case Uart_INTERFACE:
        {
            while(Serial.available() && *state != END)
            {
                read_byte = Serial.read();
                *state = parse_input(read_byte, command, *state);
            }
        }
        break;

        default:
        break;
    };

    *end = clock();
    int msec = (*end - command->begin) * 1000 / CLOCKS_PER_SEC;

    //if timeout reset state
    if(*state != SYNC && msec > RS485_TIMEOUT_TIME_MS) //timeout
    {
        Serial.printf("TIMEOUT\n"); //debug
        *state = SYNC;
        
        *error = CMD_READ_TIMEOUT;

        return NULL;
    }
    //if bad cr reset state
    else if(*state == END && command->id == DEFAULT_ID /* && check_crc(&command) */)
    {
        Serial.printf("got message\n");
        *state = SYNC;

        *error = CMD_READ_OK;

        log_command(command);
        return command;
    }
    else if(*state == END)
    {
        Serial.printf("crc error\n");
        *state = SYNC;

        *error = CMD_READ_BAD_CRC;
        return NULL;
    }
    else //default
    {
        *error = CMD_READ_NO_CMD;
        return NULL;
    }
}