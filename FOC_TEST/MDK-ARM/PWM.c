#include "PWM.h"
#include "main.h"

int16_t  phase_a_adc_offset = 0;
int16_t  phase_b_adc_offset = 0;
uint16_t adc_buff[2];

void PWMC_init(void){

    HAL_ADC_Start_IT(&hadc1);
    HAL_ADC_Start_IT(&hadc2);
    HAL_Delay(1);
    __HAL_DBGMCU_FREEZE_TIM1();
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, HALF_PWM_PERIOD_CYCLES / 2);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, HALF_PWM_PERIOD_CYCLES / 2);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, HALF_PWM_PERIOD_CYCLES / 2);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
    __HAL_TIM_ENABLE(&htim1);
}
void PWMC_SwitchOnPWM(void){
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, HALF_PWM_PERIOD_CYCLES / 2);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, HALF_PWM_PERIOD_CYCLES / 2);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, HALF_PWM_PERIOD_CYCLES / 2);
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
    while(__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_UPDATE) == RESET){

    }
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}
void PWMC_SwitchOffPWM(void){
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
    __HAL_TIM_ENABLE(&htim1);
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
    while (__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_UPDATE) == RESET)
    {
    
    }
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
}
void PWMC_TurnOnLowSides(void){
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
    while(__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_UPDATE) == RESET){

    }
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
}

int PWMC_CurrentReadingPolarization(void){
    int i=0;
    int adc_sum_a = 0;
    int adc_sum_b = 0;
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
    while(__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_UPDATE) == RESET){

    }
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
    while(i<64){
        if(__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_UPDATE) != RESET&&TIM1->CNT>HALF_PWM_PERIOD_CYCLES/2){
            __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
            i++;
            adc_sum_a += HAL_ADC_GetValue(&hadc1);
            adc_sum_b += HAL_ADC_GetValue(&hadc2);
        }
    }
    phase_a_adc_offset = adc_sum_a/i;
    phase_b_adc_offset = adc_sum_b/i;

    i=0;
    const int Vout = 2048;
    const int check_threshold = 200;
    if(phase_a_adc_offset < (Vout - check_threshold) || phase_a_adc_offset > (Vout + check_threshold)){
        i=-1;
    }
    if(phase_b_adc_offset < (Vout - check_threshold) || phase_b_adc_offset > (Vout + check_threshold)){
        i=-1;
    }
    return i;
}