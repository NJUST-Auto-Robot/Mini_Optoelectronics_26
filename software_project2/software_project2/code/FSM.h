/*
 * FSM.h
 *
 *  Created on: 2026��3��19��
 *      Author: XYY
 */

#ifndef USER_FSM_H_
#define USER_FSM_H_

#include "zf_common_headfile.h"
//#include "adc.h"

// Motor controler state
typedef enum eFSMState {
    BOOT_UP     = 0,
    IDLE        = 1,
    RUN         = 2,
    CALIBRATION = 3,
    ANTICOGGING = 4,
} tFSMState;

typedef struct sFSM
{
    tFSMState state;
    tFSMState state_next;
    uint8_t   state_next_ready;
} tFSM;

typedef struct sMCStatusword
{
    union {
        uint8_t status_code;
        struct
        {
            uint32_t switched_on : 1;
            uint32_t target_reached : 1;
            uint32_t PADDING1 : 6;
        };
    } status;

    union {
        uint8_t errors_code;
        struct
        {
            // ERROR
            uint32_t over_voltage : 1;
            uint32_t under_voltage : 1;
            uint32_t over_current : 1;
            uint32_t drv_over_tmp : 1;
            uint32_t ntc_over_tmp : 1;
            uint32_t PADDING2 : 2;

            // FATAL
            uint32_t selftest : 1;
        };
    } errors;
} tMCStatusword;

extern volatile tMCStatusword StatuswordNew;
extern volatile tMCStatusword StatuswordOld;

void      MCT_init(void);
int       MCT_reset_error(void);
tFSMState MCT_get_state(void);
int       MCT_set_state(tFSMState state);

void MCT_high_frequency_task(int id);
void MCT_low_priority_task(void);



#endif /* USER_FSM_H_ */
