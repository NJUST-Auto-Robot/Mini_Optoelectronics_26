/*
 * trapTraj.h
 *
 *  Created on: 2026��3��19��
 *      Author: XYY
 */

#ifndef USER_TRAPTRAJ_H_
#define USER_TRAPTRAJ_H_

#include "Anticogging.h"
#include "PWM.h"
#include "util.h"

typedef struct sTraj
{
    // Step
    float Y;
    float Yd;
    float Ydd;

    float start_position;
    float start_velocity;
    float end_position;

    float acc;
    float vel;
    float dec;

    float acc_distance;

    float t_acc;
    float t_vel;
    float t_dec;
    float t_total;

    uint32_t tick;

    bool profile_done;
} tTraj;

extern tTraj Traj[2];

void TRAJ_plan(tTraj *self, float position, float start_position, float start_velocity, float Vmax, float Amax, float Dmax);
void TRAJ_eval(tTraj *self);

#endif /* USER_TRAPTRAJ_H_ */
