#ifndef __PWM_CURR_H__
#define __PWM_CURR_H__

#include "main.h"
#include "stm32g4xx_hal.h"
#include "adc.h"
#include "tim.h"

#define PWM_FREQUENCY          20000
#define CURRENT_MEASURE_HZ     PWM_FREQUENCY
#define CURRENT_MEASURE_PERIOD (float) (1.0f / (float) CURRENT_MEASURE_HZ)

#define PWM_PERIOD_CYCLES       ((uint16_t)( (float) (SystemCoreClock / PWM_FREQUENCY) & 0xFFFFE))
#define HALF_PWM_PERIOD_CYCLES  ((uint16_t)( (float) (SystemCoreClock / PWM_FREQUENCY) / 2.0f ))

#define SHUNT_RESISTENCE       (0.005f)
#define V_SCALE                ((float) (16.0f * 3.3f / 4095.0f))
#define I_SCALE                ((float) ((3.3f / 4095.0f) / SHUNT_RESISTENCE / 20.0f))

extern uint16_t adc_buff[2];
extern int16_t  phase_a_adc_offset;
extern int16_t  phase_b_adc_offset;

void PWMC_init(void);
void PWMC_SwitchOnPWM(void);
void PWMC_SwitchOffPWM(void);
void PWMC_TurnOnLowSides(void);
int PWMC_CurrentReadingPolarization(void);

static inline float read_vbus(void)
{
    return 12;
}
static float read_iphase_a(void)
{
    return (float) ((int16_t)HAL_ADC_GetValue(&hadc1) - phase_a_adc_offset) * I_SCALE;
}

static float read_iphase_b(void)
{
    return (float) ((int16_t)HAL_ADC_GetValue(&hadc2) - phase_b_adc_offset) * I_SCALE;
}
static inline void set_a_duty(uint32_t duty)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty);
}

static inline void set_b_duty(uint32_t duty)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, duty);
}

static inline void set_c_duty(uint32_t duty)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, duty);
}

#endif