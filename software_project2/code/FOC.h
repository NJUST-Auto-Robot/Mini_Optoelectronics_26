/*
 * FOC.h
 *
 *  Created on: 2026��3��13��
 *      Author: XYY
 */

#ifndef USER_FOC_H_
#define USER_FOC_H_

#include "zf_common_headfile.h"

typedef struct sFOC
{
    bool is_armed;

    float v_bus, v_bus_filt, i_a, i_b, i_c;

    float i_q, i_q_filt, i_d, i_d_filt, i_bus, i_bus_filt, power_filt;
    float dtc_a, dtc_b, dtc_c;

    float current_ctrl_p_gain, current_ctrl_i_gain;
    float current_ctrl_integral_d, current_ctrl_integral_q;

    uint8_t motor_id;
} tFOC;


extern tFOC Foc[2];

void FOC_init(void);
void FOC_update_current_ctrl_gain(int id, float bw);
void FOC_arm(tFOC *self);
void FOC_disarm(tFOC *self);
void FOC_voltage(int id, float Vd_set, float Vq_set, float phase);
void FOC_current(int id, float Id_set, float Iq_set, float phase, float phase_vel);


#endif /* USER_FOC_H_ */
