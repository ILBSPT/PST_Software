#ifndef __LOG_H_
#define __LOG_H_

#include <stdbool.h>

#include <SerialFlash.h>

#include "Comms.h"

#define LOG_FILE_SIZE 1<<12 //4096 bytes

extern bool log_running;
extern SerialFlashFile flashFile;

void init_log();

void start_log();
void stop_log();

void log_command(command_t* cmd);

void show_directory();
void dump_log(int log); 
void remove_log(int log);

#endif