/*
 * PWM.c
 *
 *  Created on: 2026ùù3ùù19ùù
 *      Author: XYY
 */

#include "PWM.h"

uint32_t SystemCoreClock = AURIX_MCU_FREQUENCY;

int16_t  phase_a1_adc_offset = 0;
int16_t  phase_b1_adc_offset = 0;
int16_t  phase_a2_adc_offset = 0;
int16_t  phase_b2_adc_offset = 0;
uint16_t adc_buff[2];

void PWMC_init(void) {
    // ùù?ùùADC?ùù
    adc_init(ADC_I_C1, ADC_12BIT);
    adc_init(ADC_I_B1, ADC_12BIT);
    adc_init(ADC_I_A1, ADC_12BIT);
    adc_init(ADC_I_C2, ADC_12BIT);
    adc_init(ADC_I_B2, ADC_12BIT);
    adc_init(ADC_I_A2, ADC_12BIT);

    // ùùùùADCù?ù
    //adc_interrupt_enable(ADC_1, ADC_CHANNEL_0);
    //adc_interrupt_enable(ADC_1, ADC_CHANNEL_1);

    // ùù?ùùPWM
    pwm_init(PWM_C1,20000,5000);
    pwm_init(PWM_B1,20000,5000);
    pwm_init(PWM_A1,20000,5000);
    pwm_init(PWM_C2,20000,5000);
    pwm_init(PWM_B2,20000,5000);
    pwm_init(PWM_A2,20000,5000);

    // ùù?1ms
    system_delay_ms(1);

    // ??PWMùùù
    pwm_set_duty(PWM_C1, 0);
    pwm_set_duty(PWM_B1, 0);
    pwm_set_duty(PWM_A1, 0);
    pwm_set_duty(PWM_C2, 0);
    pwm_set_duty(PWM_B2, 0);
    pwm_set_duty(PWM_A2, 0);

    system_delay_ms(100);                      // ù?ùù?ùù
    phase_a1_adc_offset = adc_convert(ADC_I_A1);
    phase_b1_adc_offset = adc_convert(ADC_I_B1);
    phase_a2_adc_offset = adc_convert(ADC_I_A2);
    phase_b2_adc_offset = adc_convert(ADC_I_B2);

}

void PWMC_SwitchOnPWM(void) {
    pwm_set_duty(PWM_C1, HALF_PWM_PERIOD_CYCLES / 2);
    pwm_set_duty(PWM_B1, HALF_PWM_PERIOD_CYCLES / 2);
    pwm_set_duty(PWM_A1, HALF_PWM_PERIOD_CYCLES / 2);
    pwm_set_duty(PWM_C2, HALF_PWM_PERIOD_CYCLES / 2);
    pwm_set_duty(PWM_B2, HALF_PWM_PERIOD_CYCLES / 2);
    pwm_set_duty(PWM_A2, HALF_PWM_PERIOD_CYCLES / 2);

    // ù?ùùùù±ù?
//    while(__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_UPDATE) == RESET){
//
//        }
//        __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
//
//    pwm_start(PWM_1_MODULE0, PWM_CHANNEL_0);
//    pwm_start(PWM_1_MODULE0, PWM_CHANNEL_1);
//    pwm_start(PWM_1_MODULE0, PWM_CHANNEL_2);
}

void PWMC_SwitchOffPWM(void) {
    pwm_set_duty(PWM_C1, 0);
    pwm_set_duty(PWM_B1, 0);
    pwm_set_duty(PWM_A1, 0);
    pwm_set_duty(PWM_C2, 0);
    pwm_set_duty(PWM_B2, 0);
    pwm_set_duty(PWM_A2, 0);

//    pwm_enable(PWM_1_MODULE0);
//
//    while(!pwm_get_update_flag(PWM_1_MODULE0));
//    pwm_clear_update_flag(PWM_1_MODULE0);
}

void PWMC_TurnOnLowSides(void) {
    pwm_set_duty(PWM_C1, 0);
    pwm_set_duty(PWM_B1, 0);
    pwm_set_duty(PWM_A1, 0);
    pwm_set_duty(PWM_C2, 0);
    pwm_set_duty(PWM_B2, 0);
    pwm_set_duty(PWM_A2, 0);

//    while(!pwm_get_update_flag(PWM_1_MODULE0));
//    pwm_clear_update_flag(PWM_1_MODULE0);
//
//    pwm_start(PWM_1_MODULE0, PWM_CHANNEL_0);
//    pwm_start(PWM_1_MODULE0, PWM_CHANNEL_1);
//    pwm_start(PWM_1_MODULE0, PWM_CHANNEL_2);
}

int PWMC_CurrentReadingPolarization(void) {
        int adc_sum_a1 = 0;
        int adc_sum_b1 = 0;
        int adc_sum_a2 = 0;
        int adc_sum_b2 = 0;

        // ===== ùùùù?ùùù?ùù?ùADCùù=====
        for(int k = 0; k < 10; k++)
        {
            adc_convert(ADC_I_A1);
            adc_convert(ADC_I_B1);
            adc_convert(ADC_I_A2);
            adc_convert(ADC_I_B2);
            system_delay_us(50);
        }

        // ===== 64ù?ùùù =====
        for(int i = 0; i < 64; i++)
        {
            // ù?ùùùù?ùPWMù??ùù20kHz ùù 25usùù
            system_delay_us(25);

            adc_sum_a1 += adc_convert(ADC_I_A1);
            adc_sum_b1 += adc_convert(ADC_I_B1);
            adc_sum_a2 += adc_convert(ADC_I_A2);
            adc_sum_b2 += adc_convert(ADC_I_B2);

            // ùùùù?ùùùùùù
            system_delay_us(25);
        }

        phase_a1_adc_offset = adc_sum_a1 / 64;
        phase_b1_adc_offset = adc_sum_b1 / 64;
        phase_a2_adc_offset = adc_sum_a2 / 64;
        phase_b2_adc_offset = adc_sum_b2 / 64;

        // ===== ?ùù =====
        int ret = 0;
        const int Vout = 2048;
        const int check_threshold = 200;

        if(phase_a1_adc_offset < (Vout - check_threshold) || phase_a1_adc_offset > (Vout + check_threshold))
            ret = -1;

        if(phase_b1_adc_offset < (Vout - check_threshold) || phase_b1_adc_offset > (Vout + check_threshold))
            ret = -1;

        if(phase_a2_adc_offset < (Vout - check_threshold) || phase_a2_adc_offset > (Vout + check_threshold))
            ret = -1;

        if(phase_b2_adc_offset < (Vout - check_threshold) || phase_b2_adc_offset > (Vout + check_threshold))
            ret = -1;

        return ret;
}
