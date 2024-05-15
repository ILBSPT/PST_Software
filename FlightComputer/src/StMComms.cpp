#include <Arduino.h>

#include "GlobalVars.h"
#include "Comms.h"

#include "StMComms.h"

int run_command(command_t* cmd, rocket_state_t state, interface_t interface)
{
    command_t command_rep;
    command_rep.id = DEFAULT_ID;
    rocket_state_t return_state = state;
    switch(cmd->cmd)
    {
        case CMD_LED_ON:
        {
            /* Cmd execution */
            digitalWrite(LED_PIN, HIGH);

            /*
             * Prepare ACK response
             * Send response
             */
            command_rep.cmd = CMD_LED_ON_ACK;
            command_rep.size = 0;
            command_rep.crc = 0x3131;

            write_command(&command_rep, interface);

            return CMD_RUN_OK;
        }
        break;

        case CMD_LED_OFF:
        {
            /* Cmd execution */
            digitalWrite(LED_PIN, LOW);

            /*
             * Prepare ACK response
             * Send response
             */
            command_rep.cmd = CMD_LED_OFF_ACK;
            command_rep.size = 0;
            command_rep.crc = 0x2121;

            write_command(&command_rep, interface);

            return CMD_RUN_OK;
        }
        break;

        case CMD_STATUS:
        {
            /*
             * Prepare ACK response
             * Send response
             */
            command_rep.cmd = CMD_STATUS_ACK;
            //command_rep.size = 2*6 + 1;
            //command_rep.size = 100; //test
            //command_rep.data[0] = state;
            //command_rep.data[1] = (imu_ax >> 8) & 0xff;
            //command_rep.data[2] = (imu_ax) & 0xff ;
            //command_rep.data[3] = (imu_ay >> 8) & 0xff;
            //command_rep.data[4] = (imu_ay) & 0xff;
            //command_rep.data[5] = (imu_az >> 8) & 0xff;
            //command_rep.data[6] = (imu_az) & 0xff;
            //command_rep.data[7] = (imu_gx >> 8) & 0xff;
            //command_rep.data[8] = (imu_gx) & 0xff;
            //command_rep.data[9] = (imu_gy >> 8) & 0xff;
            //command_rep.data[10] = (imu_gy) & 0xff;
            //command_rep.data[11] = (imu_gz >> 8) & 0xff;
            //command_rep.data[12] = (imu_gz) & 0xff;
            command_rep.crc = 0x5151;
            
            command_rep.size = 5;
            command_rep.data[0] = state;
            command_rep.data[1] = (tank_pressure >> 8) & 0xff;
            command_rep.data[2] = (tank_pressure) & 0xff;
            command_rep.data[3] = (tank_liquid >> 8) & 0xff;
            command_rep.data[4] = (tank_liquid) & 0xff;

            write_command(&command_rep, interface);

            return CMD_RUN_OK;
        }
        break; 

        case CMD_ARM:
        {
            //stage 1
            if(cmd->data[0] != ARN_TRIGGER_1)
            {
                //error
                return CMD_RUN_ARM_ERROR;
            }

            command_rep.cmd = CMD_ARM_ACK;
            command_rep.size = 1;

            command_rep.data[0] = ARN_TRIGGER_1;
            command_rep.crc = 0x5151;

            write_command(&command_rep, interface);

            //stage 2
            int error = 0;
            command_t* arm_cmd;
            while((arm_cmd = read_command(&error, DEFAULT_CMD_INTERFACE)) == NULL && error == CMD_READ_NO_CMD) {}

            if(error != OK || arm_cmd->cmd != CMD_ARM || arm_cmd->data[0] != ARN_TRIGGER_2)
            {
                //error
                return CMD_RUN_ARM_ERROR;
            }

            command_rep.data[0] = ARN_TRIGGER_2;
            write_command(&command_rep, interface);

            //stage 3
            while((arm_cmd = read_command(&error, DEFAULT_CMD_INTERFACE)) == NULL && error == CMD_READ_NO_CMD) {}

            if(error != OK || arm_cmd->cmd != CMD_ARM || arm_cmd->data[0] != ARN_TRIGGER_3)
            {
                //error
                return CMD_RUN_ARM_ERROR;
            }

            command_rep.data[0] = ARN_TRIGGER_3;
            write_command(&command_rep, interface);

            return CMD_RUN_OK;
        }
        break;

        case CMD_READY:
        {
            command_rep.cmd = CMD_READY_ACK;
            command_rep.size = 0;
            command_rep.crc = 0x2121;

            write_command(&command_rep, interface);

            return CMD_RUN_OK;
        }
        break;

        case CMD_ABORT:
        {
            command_rep.cmd = CMD_ABORT_ACK;
            command_rep.size = 0;
            command_rep.crc = 0x2121;

            write_command(&command_rep, interface);

            return CMD_RUN_OK;
        }
        break;

        case CMD_EXEC_PROG:
        {
            if(cmd->size != 11) //1 byte for prog 2 bytes per var (p1,p2,p3,l1,l2)
            {
                return CMD_RUN_OUT_OF_BOUND;
            }

            //ensure that the rocket is in fueling mode before running a fueling program
            if(state == FUELING)
            {
                rocket_state_t next_state = -1;
                switch(cmd->data[0])
                {
                    case 1:
                        next_state = PROG1;
                        RP1 = (cmd->data[1] << 8) + cmd->data[2];
                        RP2 = (cmd->data[3] << 8) + cmd->data[4];

                    break;
                    case 2:
                        next_state = PROG2;
                        RP3 = (cmd->data[5] << 8) + cmd->data[6];
                    break;
                }

                comm_transition[FUELING][CMD_EXEC_PROG] = next_state;
            }
        }

        default:
            // if the command has no action it still needs to return ok to change state
            if(cmd->cmd < cmd_size)
                return CMD_RUN_OK;
            else //cmd code out of bounds, return error
                return CMD_RUN_OUT_OF_BOUND;
        break;
    };

    return CMD_RUN_OUT_OF_BOUND;

    
    //Serial.printf("cmd: %x state: %d return state %d table: %d\n", cmd->cmd, state, return_state,
    //(rocket_state_t)comm_transition[state][cmd->cmd]);
}