#include "FlashLog.h"

#include "HardwareCfg.h"

bool log_running = false;
File file;

uint8_t current_id;
uint32_t data_used;

void start_log()
{ 
    log_running = true; 
    
    /*
     * TODO
     *   before opening verify that the file does not exist, 
     *   256 diferent reboots before we are out of files 
    */
    char filename[100] = "";
    int count = sprintf(filename, LOG_NAME_PATTERN, current_id);
    current_id++;
    Serial.print("new file \n");
    Serial.print(filename);
    Serial.print("\n");

    file = SD.open(filename, FILE_WRITE, true);

    if(!file)
    {
        assert(false);
    }
}
void stop_log() 
{ 
    log_running = false; 
    file.close();
}

static uint8_t get_last_id()
{
    File root = SD.open("/");
    uint8_t last_log_id = 0;
    while(1)
    {
        File entry = root.openNextFile();

        if(!entry)
            break; //no more files

        if(!entry.isDirectory()) 
        {
            uint8_t temp;
            sscanf(entry.name(), "%u", &temp);
            Serial.print(temp);
            last_log_id = max(last_log_id, temp);
        }
        Serial.print(" Name in sd: ");
        Serial.print(entry.name());
        Serial.print(" ");
        Serial.print(entry.isDirectory());
        Serial.print(" \n");

        entry.close();
    }

    root.close();

    return last_log_id;
}

void init_log()
{
    bool created = false;

    if (!SD.begin(Flash_SS_PIN)) {
        printf("Unable to access SPI Flash chip\n");
        return;
    }

    current_id = get_last_id() + 1;
    Serial.print("current id");
    Serial.print(current_id);
    Serial.printf("\n");

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

    file.write(buff, index);
}


void dump_log(uint8_t id)
{
    if(log_running)
    {
        printf("error, cannot dump flash while logging\n");
        return;
    }

    char filename[100];
    sprintf(filename, LOG_NAME_PATTERN, id);

    File flashDump = SD.open(filename);
    if(!flashDump)
    {
        printf("error openening log file\n");
        return;
    }

    delay(50);
    //first send the size of the file
    Serial.write((flashDump.size() >> 24) & 0xff);
    Serial.write((flashDump.size() >> 16) & 0xff);
    Serial.write((flashDump.size() >> 8) & 0xff);
    Serial.write((flashDump.size() & 0xff));
    

    //Serial.print(flashDump.size());

    uint8_t buff[LOG_FILE_SIZE];
    while(flashDump.available())
    {
        Serial.write(flashDump.read());
    }

    flashDump.close();
}

void get_log_ids(uint8_t* files, uint8_t* files_index)
{
    *files_index = 0;

    File root = SD.open("/");
    while(1)
    {
        File entry = root.openNextFile();
        if(!entry)
            break; //no more files

        if(!entry.isDirectory()) 
        {
            uint8_t temp;
            sscanf(entry.name(), "%u", &temp);
            *(files + (*files_index)) = temp;
            //Serial.printf("%u\n", *(files + (*files_index)));
            (*files_index)++;
        }

        Serial.print(" Name in sd: ");
        Serial.print(entry.name());
        Serial.print(" ");
        Serial.print(*files_index);
        Serial.print(" size: ");
        Serial.print(entry.size());
        Serial.print(" \n");

        entry.close();
    }

    root.close();

}