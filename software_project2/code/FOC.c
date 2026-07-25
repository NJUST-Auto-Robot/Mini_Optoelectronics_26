/*
 * FOC.c
 *
 *  Created on: 2026��3��13��
 *      Author: XYY
 */
#include "FOC.h"
#include "PWM.h"
#include "util.h"
#include "IfxCpu_Intrinsics.h"

tFOC Foc[2];

void FOC_update_current_ctrl_gain(int id, float bw)
{
    float bandwidth         = bw * M_2PI;
    Foc[id].current_ctrl_p_gain = UsrConfig.motor_phase_inductance * bandwidth;
    Foc[id].current_ctrl_i_gain = UsrConfig.motor_phase_resistance / UsrConfig.motor_phase_inductance;
    if(Foc[id].current_ctrl_i_gain > 2000.0f) {
        Foc[id].current_ctrl_i_gain = 2000.0f;
    }
}
void FOC_init(void)
{
    for (int i = 0; i < 2; i++)
    {
        Foc[i].is_armed = false;

        Foc[i].v_bus = 0.0f;
        Foc[i].v_bus_filt = 0.0f;
        Foc[i].i_q = 0.0f;
        Foc[i].i_q_filt = 0.0f;
        Foc[i].i_bus = 0.0f;
        Foc[i].i_bus_filt = 0.0f;
        Foc[i].power_filt = 0.0f;
        FOC_update_current_ctrl_gain(i, UsrConfig.current_ctrl_bw);
    }
    

}
void FOC_arm(tFOC *self)
{
    if (self->is_armed) {
        return;
    }

    __disable();

    self->i_q        = 0;
    self->i_q_filt   = 0;
    self->i_bus      = 0;
    self->i_bus_filt = 0;
    self->power_filt = 0;

    self->current_ctrl_integral_d = 0;
    self->current_ctrl_integral_q = 0;

    PWMC_TurnOnLowSides();

    self->is_armed = true;

    __enable();
}
void FOC_disarm(tFOC *self)
{
    if (!self->is_armed) {
        return;
    }

    __disable();

    self->i_q        = 0;
    self->i_q_filt   = 0;
    self->i_bus      = 0;
    self->i_bus_filt = 0;
    self->power_filt = 0;

    PWMC_SwitchOffPWM();

    self->is_armed = false;

    __enable();
}
void FOC_voltage(int id, float Vd_set, float Vq_set, float phase)
{
    float v_to_mod = 1.5f / Foc[id].v_bus_filt; // = 1.0f / (Foc.v_bus_filt * 2.0f / 3.0f);
    float mod_vd   = Vd_set * v_to_mod;
    float mod_vq   = Vq_set * v_to_mod;

    // Vector modulation saturation, lock integrator if saturated
    float factor = 0.9f * SQRT3_BY_2 / sqrtf(SQ(mod_vd) + SQ(mod_vq));
    if (factor < 1.0f) {
        mod_vd *= factor;
        mod_vq *= factor;
    }

    // Inverse park transform
    float alpha, beta;
    float pwm_phase = phase;
    inverse_park(mod_vd, mod_vq, pwm_phase, &alpha, &beta);

    if (id == 0)
    {
        if (0 == svm(alpha, beta, &Foc[id].dtc_a, &Foc[id].dtc_b, &Foc[id].dtc_c))
        {
            pwm_set_duty(PWM_A1, (uint16_t)(Foc[id].dtc_a*(float)HALF_PWM_PERIOD_CYCLES));
            pwm_set_duty(PWM_B1, (uint16_t)(Foc[id].dtc_b*(float)HALF_PWM_PERIOD_CYCLES));
            pwm_set_duty(PWM_C1, (uint16_t)(Foc[id].dtc_c*(float)HALF_PWM_PERIOD_CYCLES));
        }
    }
    else
    {
        if (0 == svm(alpha, beta, &Foc[id].dtc_a, &Foc[id].dtc_b, &Foc[id].dtc_c)) 
        {
            pwm_set_duty(PWM_A2, (uint16_t)(Foc[id].dtc_a*(float)HALF_PWM_PERIOD_CYCLES));
            pwm_set_duty(PWM_B2, (uint16_t)(Foc[id].dtc_b*(float)HALF_PWM_PERIOD_CYCLES));
            pwm_set_duty(PWM_C2, (uint16_t)(Foc[id].dtc_c*(float)HALF_PWM_PERIOD_CYCLES));
        }
    }
}
void FOC_current(int id, float Id_set, float Iq_set, float phase, float phase_vel)
{
    // Clarke transform
    float i_alpha, i_beta;
    clarke_transform(Foc[id].i_a, Foc[id].i_b, Foc[id].i_c, &i_alpha, &i_beta);

    // Park transform
    float i_d, i_q;
    park_transform(i_alpha, i_beta, phase, &i_d, &i_q);

    // Current PI control
    float i_d_err = Id_set - i_d;
    float i_q_err = Iq_set - i_q;
    float v_d     = i_d_err * Foc[id].current_ctrl_p_gain + Foc[id].current_ctrl_integral_d;
    float v_q     = i_q_err * Foc[id].current_ctrl_p_gain + Foc[id].current_ctrl_integral_q;

    // voltage normalize = 1/(2/3*v_bus)
    float v_to_mod = 1.5f / Foc[id].v_bus_filt;
    float mod_vd   = v_d * v_to_mod;
    float mod_vq   = v_q * v_to_mod;

    // Vector modulation saturation, lock integrator if saturated
    float factor = 0.9f * SQRT3_BY_2 / sqrtf(SQ(mod_vd) + SQ(mod_vq));
    if (factor < 1.0f) {
        mod_vd *= factor;
        mod_vq *= factor;
        Foc[id].current_ctrl_integral_d *= 0.99f;
        Foc[id].current_ctrl_integral_q *= 0.99f;
    } else {
        Foc[id].current_ctrl_integral_d += i_d_err * (Foc[id].current_ctrl_i_gain * CURRENT_MEASURE_PERIOD);
        Foc[id].current_ctrl_integral_q += i_q_err * (Foc[id].current_ctrl_i_gain * CURRENT_MEASURE_PERIOD);
    }

    // Inverse park transform
    float alpha, beta;
    float pwm_phase = phase + phase_vel * CURRENT_MEASURE_PERIOD;
    inverse_park(mod_vd, mod_vq, pwm_phase, &alpha, &beta);

    // SVM
    if (id == 0)
    {
        if (0 == svm(alpha, beta, &Foc[id].dtc_a, &Foc[id].dtc_b, &Foc[id].dtc_c))
        {
            pwm_set_duty(PWM_A1, (uint16_t)(Foc[id].dtc_a*(float)HALF_PWM_PERIOD_CYCLES));
            pwm_set_duty(PWM_B1, (uint16_t)(Foc[id].dtc_b*(float)HALF_PWM_PERIOD_CYCLES));
            pwm_set_duty(PWM_C1, (uint16_t)(Foc[id].dtc_c*(float)HALF_PWM_PERIOD_CYCLES));
        }
    }
    else
    {
        if (0 == svm(alpha, beta, &Foc[id].dtc_a, &Foc[id].dtc_b, &Foc[id].dtc_c)) 
        {
            pwm_set_duty(PWM_A2, (uint16_t)(Foc[id].dtc_a*(float)HALF_PWM_PERIOD_CYCLES));
            pwm_set_duty(PWM_B2, (uint16_t)(Foc[id].dtc_b*(float)HALF_PWM_PERIOD_CYCLES));
            pwm_set_duty(PWM_C2, (uint16_t)(Foc[id].dtc_c*(float)HALF_PWM_PERIOD_CYCLES));
        }
    }
    
    // used for report
    Foc[id].i_q = i_q;
    UTILS_LP_FAST(Foc[id].i_q_filt, Foc[id].i_q, 0.01f);
    Foc[id].i_d = i_d;
    UTILS_LP_FAST(Foc[id].i_d_filt, Foc[id].i_d, 0.01f);
    Foc[id].i_bus = (mod_vd * i_d + mod_vq * i_q);
    UTILS_LP_FAST(Foc[id].i_bus_filt, Foc[id].i_bus, 0.01f);
    Foc[id].power_filt = Foc[id].v_bus_filt * Foc[id].i_bus_filt;

}