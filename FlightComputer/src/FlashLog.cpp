#include "FlashLog.h"

#include "HardwareCfg.h"

bool log_running = false;
SerialFlashFile flashFile;

uint8_t current_id;
uint32_t data_used;

void start_log() { log_running = true; }
void stop_log() { log_running = false; }

static uint8_t get_last_id()
{
    uint8_t last_log_id = -1;
    SerialFlash.opendir();
    while (1) {
        char filename[64];
        uint32_t filesize;

        if (SerialFlash.readdir(filename, sizeof(filename), filesize)) {
            uint8_t temp;
            sscanf(filename, LOG_NAME_PATTERN, &temp);
            last_log_id = max(last_log_id, temp);
            
        } else {
            break; // no more files
        }
    }

    return last_log_id;
}

static bool new_log_file(SerialFlashFile* file)
{
    char filename[100];
    sprintf(filename, LOG_NAME_PATTERN, current_id);
    if(!SerialFlash.create(filename, LOG_FILE_SIZE, 0))
    {
        printf("error creating log file");
        return false;
    }
     
    *file = SerialFlash.open(filename);
    if(!flashFile)
    {
        printf("error opening log file");
        return false;
    }
    
    current_id++;
    data_used = 0;
    return true;

}

void init_log()
{
    bool created = false;

    if (!SerialFlash.begin(Flash_SS_PIN)) {
        printf("Unable to access SPI Flash chip");
        return;
    }

    current_id = get_last_id() + 1;
    if(!new_log_file(&flashFile))
    {
        assert(false);
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
    
    if(data_used + index >= LOG_FILE_SIZE) 
        if(!new_log_file(&flashFile))
        {
            return;
        }

    data_used += index;
    flashFile.write(buff, index);
}


void dump_log(uint8_t id)
{
    if(log_running)
    {
        printf("error, cannot dump flash while logging\n");
        return;
    }

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

void get_log_ids(uint8_t files[256], uint8_t* files_index)
{
    *files_index = 0;
    SerialFlash.opendir();
    while (1) {
        char filename[64];
        uint32_t filesize;

        if (SerialFlash.readdir(filename, sizeof(filename), filesize)) {
            sscanf(filename, LOG_NAME_PATTERN, (files + *files_index));
            *files_index++;
        } else {
            break; // no more files
        }
    }
}