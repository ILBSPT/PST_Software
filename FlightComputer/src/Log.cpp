#include "Log.h"

#include "HardwareCfg.h"

bool log_running = false;
SerialFlashFile flashFile;
uint8_t current_id;

void start_log() { log_running = true; }
void stop_log() { log_running = false; }

void init_log()
{
    bool created = false;

    if (!SerialFlash.begin(Flash_SS_PIN)) {
        printf("Unable to access SPI Flash chip");
        return;
    }

    //flash config file still does not exit
    //create one with 1024 bytes
    if(!SerialFlash.exists("config.bin"))
    {
        if(!SerialFlash.create("config.bin", 1024, 0))
        {
            printf("error cannot create config file\n");
            return;
        }

        created = true;
    }

    SerialFlashFile flashCfgFile;
    flashCfgFile = SerialFlash.open("config.h");

    if(!flashCfgFile)
    {
        printf("Cannot opne config file\n");
        return;
    }

    //put the next log id in the config file
    if(created)
    {
        uint8_t buff[1] = {0};
        flashCfgFile.write(buff, 1);
    }

    uint8_t next_id;
    flashCfgFile.read(&next_id, 1);
    current_id = next_id;

    char filename[100];
    sprintf(filename, "%d_log.bin", next_id);
    if(!SerialFlash.create(filename, LOG_FILE_SIZE, 0))
    {
        printf("error creating log file");
        return;
    }
     
    flashFile = SerialFlash.open(filename);
    if(!flashFile)
    {
        printf("error openening log file");
        return;
    }
}

void log_command(command_t* cmd)
{

    if(log_running == false) return;

    uint32_t time = millis();
    uint8_t buff[MAX_COMMAND_BUFFER + 50] = "CMD ";
    int index = 4;

    buff[index++] = cmd->cmd;
    //buff[index++] = cmd->id;
    buff[index++] = cmd->size;
    for(int i = 0; i < cmd->size; i++) 
        buff[index++] = cmd->data[i];

    buff[index++] = (time >> 24) & 0xff;
    buff[index++] = (time >> 16) & 0xff;
    buff[index++] = (time >> 8)  & 0xff;
    buff[index++] = (time)       & 0xff;
    buff[index++] = '\n';

    flashFile.write(buff, index);
}


void dump_log(int id)
{
    char filename[100];
    sprintf(filename, "%d_log.bin", id);

    SerialFlashFile flashDump = SerialFlash.open(filename);
    if(!flashDump)
    {
        printf("error openening log file");
        return;
    }

    uint8_t buff[LOG_FILE_SIZE];
    flashDump.read(buff, LOG_FILE_SIZE);

    Serial.write(buff, LOG_FILE_SIZE);
}