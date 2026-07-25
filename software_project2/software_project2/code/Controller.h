/*
 * Controller.h
 *
 *  Created on: 2026��3��19��
 *      Author: XYY
 */

#ifndef USER_CONTROLLER_H_
#define USER_CONTROLLER_H_

#include "zf_common_headfile.h"
#include "encoder.h"
#include "foc.h"
#include "PWM.h"
#include "trapTraj.h"
#include "User_config.h"
#include "util.h"

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

    uint8_t motor_id;
} tController;


extern tController Controller[2];

int  CONTROLLER_set_op_mode(tController *self, tControlMode mode);
int  CONTROLLER_set_home(tController *self);
void CONTROLLER_sync_callback(tController *self);
void CONTROLLER_init(void);
void CONTROLLER_update_input_pos_filter_gain(tController *self, float bw);
void CONTROLLER_reset(tController *self);
void CONTROLLER_loop(int id);

#endif /* USER_CONTROLLER_H_ */
