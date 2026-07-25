/*
 * all.h
 *
 *  Created on: 2026��6��22��
 *      Author: bob
 */

#ifndef ALL_H_
#define ALL_H_

#include "Encoder.h"
#include "PWM.h"
#include "FSM.h"
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

#define CAR_CENTRE 105            //��ͷ�еĳ������Ķ�Ӧ������

/*��������״̬��ö�ٱ�������*/
typedef enum
{
    SIMPLE_TRACK = 0,
    LEFT_CURVE = 1,         //��ֱ����
    RIGHT_CURVE = 2,        //��ֱ����
    T_FORK = 3,             //��T�Ͳ�·
    LEFT_T_FORK = 4,        //��T�Ͳ�·
    RIGHT_T_FORK = 5,       //��T�Ͳ�·
}track_attribute_enum;

//extern int16 Start_Track_Err;
//extern int16 Qianzhan_Track_Angle;
extern volatile track_attribute_enum track_attribute;


#endif /* ALL_H_ */
