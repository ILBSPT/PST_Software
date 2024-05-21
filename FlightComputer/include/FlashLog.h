#ifndef __LOG_H_
#define __LOG_H_

#include <stdbool.h>

//#include <SerialFlash.h>
#include <SD.h>

#include "Comms.h"

#define LOG_FILE_SIZE 1<<12 //4096 bytes
#define LOG_NAME_PATTERN "/%u_log.bin"

extern bool log_running;
extern File file;

extern uint8_t current_id;

void start_log();
void stop_log();

void log_command(command_t* cmd);

void dump_log(uint8_t log); 
void get_log_ids(uint8_t* files, uint8_t *file_index);

uint8_t get_last_id();

#endif