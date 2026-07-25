/*
 * PWM.h
 *
 *  Created on: 2026��3��19��
 *      Author: XYY
 */

#ifndef USER_PWM_H_
#define USER_PWM_H_
#include "zf_common_headfile.h"

#define ADC_I_A1    ADC0_CH2_A2
#define ADC_I_B1    ADC0_CH1_A1
#define ADC_I_C1    ADC0_CH0_A0
#define ADC_I_A2    ADC8_CH5_A37
#define ADC_I_B2    ADC8_CH4_A36
#define ADC_I_C2    ADC8_CH3_A35
#define PWM_A1  ATOM0_CH6_P02_6
#define PWM_B1  ATOM0_CH5_P02_5
#define PWM_C1  ATOM0_CH4_P02_4
#define PWM_A2  ATOM2_CH7_P13_2
#define PWM_B2  ATOM2_CH6_P13_1
#define PWM_C2  ATOM2_CH5_P13_0
#define PWM_MOTOR_A(id)  ((id) == 0 ? PWM_A1 : PWM_A2)
#define PWM_MOTOR_B(id)  ((id) == 0 ? PWM_B1 : PWM_B2)
#define PWM_MOTOR_C(id)  ((id) == 0 ? PWM_C1 : PWM_C2)

extern uint32_t SystemCoreClock;

#define PWM_FREQUENCY          20000
#define CURRENT_MEASURE_HZ     PWM_FREQUENCY
#define CURRENT_MEASURE_PERIOD (float) (1.0f / (float) CURRENT_MEASURE_HZ)

#define PWM_PERIOD_CYCLES       ((uint16_t)( (float) (SystemCoreClock / PWM_FREQUENCY) & 0xFFFFE))
#define HALF_PWM_PERIOD_CYCLES  ((uint16_t)( (float) (SystemCoreClock / PWM_FREQUENCY) / 2.0f ))

#define SHUNT_RESISTENCE       (0.01f)
#define V_SCALE                ((float) (16.0f * 3.3f / 4095.0f))
#define I_SCALE                ((float) ((3.3f / 4095.0f) / SHUNT_RESISTENCE / 20.0f))

extern uint16_t adc_buff[2];
extern int16_t  phase_a1_adc_offset;
extern int16_t  phase_b1_adc_offset;
extern int16_t  phase_a2_adc_offset;
extern int16_t  phase_b2_adc_offset;

void PWMC_init(void);
void PWMC_SwitchOnPWM(void);
void PWMC_SwitchOffPWM(void);
void PWMC_TurnOnLowSides(void);
int PWMC_CurrentReadingPolarization(void);

static inline float read_vbus(void)
{
    return 12;
}

static inline float read_iphase_a(uint8_t motor_id)
{
    if (motor_id == 0)
        return (float)((int16_t)adc_convert(ADC_I_A1) - phase_a1_adc_offset) * I_SCALE;
    else
        return (float)((int16_t)adc_convert(ADC_I_A2) - phase_a2_adc_offset) * I_SCALE;
}
static inline float read_iphase_b(uint8_t motor_id)
{
    if (motor_id == 0)
        return (float)((int16_t)adc_convert(ADC_I_B1) - phase_b1_adc_offset) * I_SCALE;
    else
        return (float)((int16_t)adc_convert(ADC_I_B2) - phase_b2_adc_offset) * I_SCALE;
}
static inline void set_a_duty(uint32_t duty)
{
    pwm_set_duty(PWM_A1, duty);
}

static inline void set_b_duty(uint32_t duty)
{
    pwm_set_duty(PWM_B1, duty);
}

static inline void set_c_duty(uint32_t duty)
{
    pwm_set_duty(PWM_C1, duty);
}



#endif /* USER_PWM_H_ */
