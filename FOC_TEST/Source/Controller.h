#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "main.h"
#include "stm32g4xx_hal.h"
#include "anticogging.h"
#include "encoder.h"
#include "foc.h"
#include "FSM.h"
#include "PWM.h"
#include "trapTraj.h"
#include "User_config.h"
#include "util.h"
#include <stdbool.h>

typedef enum {
    CONTROL_MODE_CURRENT_RAMP     = 0,
    CONTROL_MODE_VELOCITY_RAMP    = 1,
    CONTROL_MODE_POSITION_FILTER  = 2,
    CONTROL_MODE_POSITION_PROFILE = 3,
} tControlMode;

typedef struct sController
{
    int   ctrl_mode;
    float input_position;
    float input_velocity;
    float input_current;

    float input_position_buffer;
    float input_velocity_buffer;
    float input_current_buffer;

    float pos_setpoint;
    float vel_setpoint;
    float cur_setpoint;

    bool  input_updated;
    float input_pos_filter_kp;
    float input_pos_filter_ki;
    float vel_integrator;
} tController;

extern tController Controller;

int  CONTROLLER_set_op_mode(tControlMode mode);
int  CONTROLLER_set_home(void);
void CONTROLLER_sync_callback(void);

void CONTROLLER_init(void);
void CONTROLLER_update_input_pos_filter_gain(float bw);
void CONTROLLER_reset(void);
void CONTROLLER_loop(void);

#endif