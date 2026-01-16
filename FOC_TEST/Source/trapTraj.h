
#ifndef _TRAP_TRAJ_H
#define _TRAP_TRAJ_H

#include "main.h"
#include <stdbool.h>
#include "PWM.h"
#include "util.h"
#include <math.h>

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

extern tTraj Traj;

void TRAJ_plan(float position, float start_position, float start_velocity, float Vmax, float Amax, float Dmax);
void TRAJ_eval(void);

#endif
