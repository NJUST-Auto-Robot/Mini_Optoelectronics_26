/*********************************************************************************************************************
* TC364 Opensourec Library ����TC364 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� TC364 ��Դ���һ����
*
* TC364 ��Դ�� ���������
* �����Ը���������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù�������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ����֤Э�� ������������Ϊ���İ汾
* ��������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ����֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          zf_driver_pwm
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.10.2
* ����ƽ̨          TC364DP
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-11-02       pudding            first version
********************************************************************************************************************/

#ifndef _zf_driver_pwm_h_
#define _zf_driver_pwm_h_

#include "zf_common_typedef.h"

#define PWM_DUTY_MAX     10000                 // PWM���ռ�ձ�  ���ռ�ձ�Խ��ռ�ձȵĲ���ֵԽС

// ��ö�ٶ��岻�����û��޸�
typedef enum // ö��PWM����
{
    ATOM0_CH0_P00_0,  ATOM0_CH0_P02_0,  ATOM0_CH0_P02_8,  ATOM0_CH0_P14_5,  ATOM0_CH0_P21_2,  ATOM0_CH0_P22_1,
    ATOM0_CH1_P00_1,  ATOM0_CH1_P00_2,  ATOM0_CH1_P02_1,  ATOM0_CH1_P14_4,  ATOM0_CH1_P21_3,  ATOM0_CH1_P22_0,  ATOM0_CH1_P33_9,
    ATOM0_CH2_P00_3,  ATOM0_CH2_P02_2,  ATOM0_CH2_P14_3,  ATOM0_CH2_P21_4,  ATOM0_CH2_P33_11,
    ATOM0_CH3_P00_4,  ATOM0_CH3_P02_3,  ATOM0_CH3_P14_2,  ATOM0_CH3_P21_5,  ATOM0_CH3_P22_2,
    ATOM0_CH4_P00_5,  ATOM0_CH4_P02_4,  ATOM0_CH4_P14_1,  ATOM0_CH4_P20_3,  ATOM0_CH4_P21_6,  ATOM0_CH4_P22_3,
    ATOM0_CH5_P00_6,  ATOM0_CH5_P02_5,  ATOM0_CH5_P21_7,  ATOM0_CH5_P32_4,
    ATOM0_CH6_P00_7,  ATOM0_CH6_P02_6,  ATOM0_CH6_P20_0,  ATOM0_CH6_P23_1,
    ATOM0_CH7_P00_8,  ATOM0_CH7_P02_7,  ATOM0_CH7_P20_8,

    ATOM1_CH0_P00_0,  ATOM1_CH0_P02_0,  ATOM1_CH0_P02_8,  ATOM1_CH0_P15_5,  ATOM1_CH0_P15_6,  ATOM1_CH0_P20_12,
    ATOM1_CH1_P00_1,  ATOM1_CH1_P00_2,  ATOM1_CH1_P02_1,  ATOM1_CH1_P10_1,  ATOM1_CH1_P14_6,  ATOM1_CH1_P15_7,  ATOM1_CH1_P15_8,  ATOM1_CH1_P20_13, ATOM1_CH1_P33_9,
    ATOM1_CH2_P00_3,  ATOM1_CH2_P02_2,  ATOM1_CH2_P10_2,  ATOM1_CH2_P10_5,  ATOM1_CH2_P14_0,  ATOM1_CH2_P20_14, ATOM1_CH2_P33_11,
    ATOM1_CH3_P00_4,  ATOM1_CH3_P02_3,  ATOM1_CH3_P10_3,  ATOM1_CH3_P10_6,  ATOM1_CH3_P15_0,
    ATOM1_CH4_P00_5,  ATOM1_CH4_P02_4,  ATOM1_CH4_P15_1,  ATOM1_CH4_P20_3,
    ATOM1_CH5_P00_6,  ATOM1_CH5_P02_5,  ATOM1_CH5_P15_2,  ATOM1_CH5_P20_9,  ATOM1_CH5_P32_4,
    ATOM1_CH6_P00_7,  ATOM1_CH6_P02_6,  ATOM1_CH6_P15_3,  ATOM1_CH6_P20_10, ATOM1_CH6_P23_1,
    ATOM1_CH7_P00_8,  ATOM1_CH7_P02_7,  ATOM1_CH7_P15_4,  ATOM1_CH7_P20_11,

    ATOM2_CH0_P00_9,  ATOM2_CH0_P13_3,  ATOM2_CH0_P33_4,  ATOM2_CH0_P33_10,
    ATOM2_CH1_P11_2,  ATOM2_CH1_P33_5,
    ATOM2_CH2_P11_3,  ATOM2_CH2_P33_6,
    ATOM2_CH3_P00_12, ATOM2_CH3_P11_6,  ATOM2_CH3_P33_7,
    ATOM2_CH4_P11_9,  ATOM2_CH4_P33_8,  ATOM2_CH4_P33_12,
    ATOM2_CH5_P11_10, ATOM2_CH5_P13_0,  ATOM2_CH5_P20_9,  ATOM2_CH5_P33_13,
    ATOM2_CH6_P11_11, ATOM2_CH6_P13_1,  ATOM2_CH6_P20_6,  ATOM2_CH6_P32_0,
    ATOM2_CH7_P11_12, ATOM2_CH7_P13_2,  ATOM2_CH7_P20_7,

    ATOM3_CH0_P00_9,  ATOM3_CH0_P13_3,  ATOM3_CH0_P33_4,  ATOM3_CH0_P33_10,
    ATOM3_CH1_P11_2,  ATOM3_CH1_P33_5,
    ATOM3_CH2_P11_3,  ATOM3_CH2_P33_6,
    ATOM3_CH3_P00_12, ATOM3_CH3_P11_6,  ATOM3_CH3_P33_7,
    ATOM3_CH4_P11_9,  ATOM3_CH4_P33_8,  ATOM3_CH4_P33_12,
    ATOM3_CH5_P11_10, ATOM3_CH5_P13_0,  ATOM3_CH5_P33_13,
    ATOM3_CH6_P11_11, ATOM3_CH6_P13_1,  ATOM3_CH6_P20_6,  ATOM3_CH6_P32_0,
    ATOM3_CH7_P11_12, ATOM3_CH7_P13_2,  ATOM3_CH7_P20_7,
}pwm_channel_enum;

//====================================================PWM ��������====================================================
void pwm_all_channel_close      (void);
void pwm_init                   (pwm_channel_enum pwmch, uint32 freq, uint32 duty);
void pwm_set_duty               (pwm_channel_enum pwmch, uint32 duty);
//====================================================PWM ��������====================================================

#endif
