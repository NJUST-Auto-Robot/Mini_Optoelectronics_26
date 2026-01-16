#ifndef __FOC_H__
#define __FOC_H__

#include "PWM.h"
#include "User_Config.h"
#include "util.h"
#include "math.h"
#include "main.h"
#include "stm32g4xx_hal.h"
#include "stdbool.h"

typedef struct sFOC
{
    bool is_armed;

    float v_bus, v_bus_filt, i_a, i_b, i_c;

    float i_q, i_q_filt, i_d, i_d_filt, i_bus, i_bus_filt, power_filt;
    float dtc_a, dtc_b, dtc_c;

    float current_ctrl_p_gain, current_ctrl_i_gain;
    float current_ctrl_integral_d, current_ctrl_integral_q;
} tFOC;


extern tFOC Foc;

void FOC_init(void);
void FOC_update_current_ctrl_gain(float bw);
void FOC_arm(void);
void FOC_disarm(void);
void FOC_voltage(float Vd_set, float Vq_set, float phase);
void FOC_current(float Id_set, float Iq_set, float phase, float phase_vel);

#endif