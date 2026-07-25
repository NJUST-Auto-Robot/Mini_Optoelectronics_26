/*
 * Receiver.h
 *
 *  Created on: 2026��3��19��
 *      Author: XYY
 */

#ifndef USER_RECEIVER_H_
#define USER_RECEIVER_H_

#include "zf_common_headfile.h"

#define RECEIVER_PACKET_HEADER_1 0x07
#define RECEIVER_PACKET_HEADER_2 0x21
#define RECEIVER_PACKET_MAX_DATA_LENGTH 128
typedef enum {
    Calibration = 0x01,
    Anticogging = 0x02,
    Save_all  = 0x03,
    Motor_on = 0x04,
    Motor_off = 0x05,
    Set_control_mode = 0x06,
} tReceiverCommand;

typedef struct
{
    uint8_t command;
    uint8_t data[128];
} tReceiverPacket;

extern void ProcessReceivedData(uint8_t* data, uint16_t length);


#endif /* USER_RECEIVER_H_ */
