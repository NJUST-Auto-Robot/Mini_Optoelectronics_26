
#include "Receiver.h"
#include "Controller.h"
#include "FSM.h"
#include "User_Config.h"
#include <string.h>

tReceiverPacket receivedPacket;
void  CONTROLLER_sync_callback(void)
{
    if (MCT_get_state() != RUN) {
        return;
    }

    switch (Controller.ctrl_mode) {
    case CONTROL_MODE_CURRENT_RAMP:
        Controller.input_current = Controller.input_current_buffer;
        break;

    case CONTROL_MODE_VELOCITY_RAMP:
        Controller.input_velocity = Controller.input_velocity_buffer;
        break;

    case CONTROL_MODE_POSITION_FILTER:
        Controller.input_position = Controller.input_position_buffer;
        break;

    case CONTROL_MODE_POSITION_PROFILE:
        Controller.input_position = Controller.input_position_buffer;
        Controller.input_updated  = true;
        break;

    default:
        break;
    }

    StatuswordNew.status.target_reached = 0;
}
int DataHeaderCheck(uint8_t* data, uint16_t length)
{
    int head_flag = 0;
    for(head_flag = 0; head_flag < length; head_flag++)
    {
        if(data[head_flag] == 0x07)
        {
            head_flag++;
            if(data[head_flag] == 0x21)
            {
                head_flag++; // 指向数据包起始位置
                return head_flag;
            }
        }
    }
    return -1;
}
int LoadData(uint8_t* data, uint16_t length)
{
    int header_index = DataHeaderCheck(data, length);
    if(header_index < 0 )
    {
        return -1; // 未找到数据包头
    }
    memcpy(&receivedPacket, &data[header_index], sizeof(tReceiverPacket));
    return 0; // 成功加载数据包
}
void Control_callback(tReceiverPacket* receivedPacket)
{
    uint8_t mode = receivedPacket->data[0];
    if(mode > CONTROL_MODE_POSITION_PROFILE)
    {
        return; // 无效模式，直接返回
    }
    CONTROLLER_set_op_mode((tControlMode)mode);
    switch(mode)
    {
        case CONTROL_MODE_CURRENT_RAMP:
        if (UsrConfig.invert_motor_dir) {
                Controller.input_current_buffer = -data_to_float(&receivedPacket->data[0]);
            } else {
                Controller.input_current_buffer = +data_to_float(&receivedPacket->data[0]);
            }
            if (!UsrConfig.sync_target_enable) {
                CONTROLLER_sync_callback();
            }
            break;
        case CONTROL_MODE_VELOCITY_RAMP:
            if (UsrConfig.invert_motor_dir) {
                Controller.input_velocity_buffer = -data_to_float(&receivedPacket->data[0]);
            } else {
                Controller.input_velocity_buffer = +data_to_float(&receivedPacket->data[0]);
            }
            if (!UsrConfig.sync_target_enable) {
                CONTROLLER_sync_callback();
            }
            break;
        case CONTROL_MODE_POSITION_FILTER:
        case CONTROL_MODE_POSITION_PROFILE:
            if (UsrConfig.invert_motor_dir) {
                Controller.input_position_buffer = -data_to_float(&receivedPacket->data[0]);
            } else {
                Controller.input_position_buffer = +data_to_float(&receivedPacket->data[0]);
            }
            if (!UsrConfig.sync_target_enable) {
                CONTROLLER_sync_callback();
            }
            break;

        default:

            break;
    }
}
void ProcessReceivedData(uint8_t* data, uint16_t length)
{
    if(LoadData(data, length) != 0)
    {
        return; // 数据包加载失败，直接返回
    }
    // 根据命令进行处理
    switch(receivedPacket.command)
    {
        case Calibration:
            MCT_set_state(CALIBRATION);
            break;
        case Anticogging:
            MCT_set_state(ANTICOGGING);
            break;
        case Save_all:
            USR_save_cogging_map();
            USR_save_config();
            break;
        case Motor_on:
            MCT_set_state(RUN);
            break;
        case Motor_off:
            MCT_set_state(IDLE);
            break;
        case Set_control_mode:
            Control_callback(&receivedPacket);
        default:
            
            break;
    }
}