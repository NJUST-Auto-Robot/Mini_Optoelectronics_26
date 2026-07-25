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
* �ļ�����          zf_driver_adc
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

#ifndef _zf_driver_adc_h
#define _zf_driver_adc_h

#include "zf_common_typedef.h"

typedef enum    // ö��ADCͨ��
{
    // ADC0��ѡ����
    ADC0_CH0_A0   = 0*16 + 0,
    ADC0_CH1_A1,
    ADC0_CH2_A2,
    ADC0_CH3_A3,
    ADC0_CH4_A4,
    ADC0_CH5_A5,
    ADC0_CH6_A6,
    ADC0_CH7_A7,

    // ADC1��ѡ����
    ADC1_CH0_A8   = 1*16 + 0,
    ADC1_CH2_A10  = 1*16 + 2,
    ADC1_CH3_A11,
    ADC1_CH4_A12,
    ADC1_CH5_A13,

    // ADC2��ѡ����
    ADC2_CH0_A16  = 2*16 + 0,
    ADC2_CH1_A17  = 2*16 + 1,
    ADC2_CH4_A20  = 2*16 + 4,
    ADC2_CH5_A21  = 2*16 + 5,

    // ADC3��ѡ����
    ADC3_CH0_A24  = 3*16 + 0,
    ADC3_CH1_A25  = 3*16 + 1,

    // ADC8��ѡ����
    ADC8_CH3_A35  = 8*16 + 3,
    ADC8_CH4_A36,
    ADC8_CH5_A37,
    ADC8_CH6_A38,
    ADC8_CH7_A39,
    ADC8_CH10_A42 = 8*16 + 10,
    ADC8_CH11_A43,
    ADC8_CH12_A44,
    ADC8_CH13_A45,
    ADC8_CH14_A46,
    ADC8_CH15_A47,
}adc_channel_enum;

// ��ö�ٶ��岻�����û��޸�
typedef enum        // ö��ADCͨ��
{
    ADC_8BIT,       // 8λ�ֱ���
    ADC_10BIT,      // 10λ�ֱ���
    ADC_12BIT,      // 12λ�ֱ���
}adc_resolution_enum;

//====================================================ADC ��������====================================================
uint16  adc_convert             (adc_channel_enum vadc_chn);                                    // ADCת��һ��
uint16  adc_mean_filter_convert (adc_channel_enum vadc_chn, uint8 count);                       // ADC��ֵ�˲�
void    adc_init                (adc_channel_enum vadc_chn, adc_resolution_enum resolution);    // ADC��ʼ��
//====================================================ADC ��������====================================================


#endif
