/*
 * Receiver.c
 *
 *  Created on: 2026��3��19��
 *      Author: XYY
 */

#include "Receiver.h"
#include "Controller.h"
#include "FSM.h"
#include "User_Config.h"
#include <string.h>

tReceiverPacket receivedPacket;

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
                head_flag++; // ָ�����ݰ���ʼλ��
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
        return -1; // δ�ҵ����ݰ�ͷ
    }
    memcpy(&receivedPacket, &data[header_index], sizeof(tReceiverPacket));
    return 0; // �ɹ��������ݰ�
}
void Control_callback(tReceiverPacket* receivedPacket, int id)
{
    uint8_t mode = receivedPacket->data[0];
    if(mode > CONTROL_MODE_POSITION_PROFILE)
    {
        return; // ��Чģʽ��ֱ�ӷ���
    }
    CONTROLLER_set_op_mode(&Controller, (tControlMode)mode);
    switch(mode)
    {
        case CONTROL_MODE_CURRENT_RAMP:
        if (UsrConfig.invert_motor_dir) {
                Controller[id].input_current_buffer = -data_to_float(&receivedPacket->data[0]);
            } else {
                Controller[id].input_current_buffer = +data_to_float(&receivedPacket->data[0]);
            }
            if (!UsrConfig.sync_target_enable) {
                CONTROLLER_sync_callback(&Controller[id]);
            }
            break;
        case CONTROL_MODE_VELOCITY_RAMP:
            if (UsrConfig.invert_motor_dir) {
                Controller[id].input_velocity_buffer = -data_to_float(&receivedPacket->data[0]);
            } else {
                Controller[id].input_velocity_buffer = +data_to_float(&receivedPacket->data[0]);
            }
            if (!UsrConfig.sync_target_enable) {
                CONTROLLER_sync_callback(&Controller[id]);
            }
            break;
        case CONTROL_MODE_POSITION_FILTER:
        case CONTROL_MODE_POSITION_PROFILE:
            if (UsrConfig.invert_motor_dir) {
                Controller[id].input_position_buffer = -data_to_float(&receivedPacket->data[0]);
            } else {
                Controller[id].input_position_buffer = +data_to_float(&receivedPacket->data[0]);
            }
            if (!UsrConfig.sync_target_enable) {
                CONTROLLER_sync_callback(&Controller[id]);
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
        return; // ���ݰ�����ʧ�ܣ�ֱ�ӷ���
    }
    // ����������д���
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
            Control_callback(&receivedPacket, 0);
            Control_callback(&receivedPacket, 1);
            break;
        default:
            break;
    }
}
