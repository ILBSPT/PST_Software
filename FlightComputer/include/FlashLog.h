#ifndef __LOG_H_
#define __LOG_H_

#include <stdbool.h>

#include <SerialFlash.h>

#include "Comms.h"

#define LOG_FILE_SIZE 1<<12 //4096 bytes
#define LOG_NAME_PATTERN "%c_log.bin"

extern bool log_running;
extern SerialFlashFile flashFile;

void init_log();

void start_log();
void stop_log();

void log_command(command_t* cmd);

void dump_log(uint8_t log); 
void get_log_ids(uint8_t files[256], uint8_t *file_index);


#endif