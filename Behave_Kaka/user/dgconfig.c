/*
 * dgconfig.c
 *
 *  Created on: 2026��7��25��
 *      Author: dragon
 */

#include "dgconfig.h"
#include "zf_common_headfile.h"

#define LED1 (P33_7)
#define LED2 (P33_8)

//************                 */
#define KEY1 (P33_12)
#define KEY2 (P33_10)
#define KEY3 (P33_11)
#define KEY4 (P33_9)

#define ADC_CHANNEL_NUMBER (6)

#define ADC_CHANNEL1 (ADC0_CH0_A0)
#define ADC_CHANNEL2 (ADC0_CH1_A1)
#define ADC_CHANNEL3 (ADC0_CH2_A2)

#define ADC_CHANNEL4 (ADC8_CH3_A35)
#define ADC_CHANNEL5 (ADC8_CH4_A36)
#define ADC_CHANNEL6 (ADC8_CH5_A37)

uint8 adc_channel_index = 0;
adc_channel_enum adc_channel_list[ADC_CHANNEL_NUMBER] =
    {
        ADC_CHANNEL1, ADC_CHANNEL2, ADC_CHANNEL3, ADC_CHANNEL4,
        ADC_CHANNEL5, ADC_CHANNEL6};

//************                 */

#define PWM_CHANNEL_NUMBER (6)

#define PWM_CH1 (ATOM0_CH4_P02_4)
#define PWM_CH2 (ATOM0_CH5_P02_5)
#define PWM_CH3 (ATOM0_CH6_P02_6)
#define PWM_CH4 (ATOM3_CH5_P13_0)
#define PWM_CH5 (ATOM3_CH6_P13_1)
#define PWM_CH6 (ATOM3_CH7_P13_2)

int16 pwm_test_duty = 0;
int16 pwm_test_duty_temp = 0;
uint8 pwm_test_channel_index = 0;
pwm_channel_enum pwm_channel_list[PWM_CHANNEL_NUMBER] = {PWM_CH1, PWM_CH2, PWM_CH3, PWM_CH4};

//*************          */
uint16 test_delay_time = 0;
uint8 led_state = 0;

uint16_t menc15a_data[2] = {0};

void Base_Peripheral_Init(void)
{

    gpio_init(LED1, GPO, GPIO_LOW, GPO_PUSH_PULL);  // ��ʼ�� LED1 ���?? Ĭ�ϸߵ�ƽ �������ģ�?
    gpio_init(LED2, GPO, GPIO_HIGH, GPO_PUSH_PULL); // ��ʼ�� LED2 ���?? Ĭ�ϸߵ�ƽ �������ģ�?

    gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP); // ��ʼ�� KEY1 ���� Ĭ�ϸߵ�ƽ ��������
    gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP); // ��ʼ�� KEY2 ���� Ĭ�ϸߵ�ƽ ��������
    gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP); // ��ʼ�� KEY3 ���� Ĭ�ϸߵ�ƽ ��������
    gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP); // ��ʼ�� KEY4 ���� Ĭ�ϸߵ�ƽ ��������

    //
    adc_init(ADC_CHANNEL1, ADC_12BIT);
    adc_init(ADC_CHANNEL2, ADC_12BIT);
    adc_init(ADC_CHANNEL3, ADC_10BIT);

    adc_init(ADC_CHANNEL4, ADC_8BIT);
    adc_init(ADC_CHANNEL5, ADC_12BIT);
    adc_init(ADC_CHANNEL6, ADC_10BIT);

    pwm_init(PWM_CH1, 17000, 0);
    pwm_init(PWM_CH2, 17000, 0);
    pwm_init(PWM_CH3, 17000, 0);
    pwm_init(PWM_CH4, 17000, 0);
    pwm_init(PWM_CH5, 17000, 0);
    pwm_init(PWM_CH6, 17000, 0);
}
void Module_Peripheral_Init()
{
    menc15a_init();
    imu963ra_init();
}

void led_key_test()
{

    test_delay_time = 300;
    if (!gpio_get_level(KEY1) || !gpio_get_level(KEY2) || !gpio_get_level(KEY3) || !gpio_get_level(KEY4))
    {
        gpio_set_level(LED1, led_state);
        gpio_set_level(LED2, led_state);
    }
    else
    {
        gpio_set_level(LED1, led_state);
        gpio_set_level(LED2, !led_state);
    }
    led_state = !led_state;

    system_delay_ms(test_delay_time);
}

void Pwm_Adc_Test()
{
    for (adc_channel_index = 0; adc_channel_index < ADC_CHANNEL_NUMBER; adc_channel_index++)
    {
        printf(
            "ADC channel %d convert data is %d.\r\n",
            adc_channel_index + 1,
            adc_convert(adc_channel_list[adc_channel_index]));
    }
    system_delay_ms(1000);

    for (adc_channel_index = 0; adc_channel_index < ADC_CHANNEL_NUMBER; adc_channel_index++)
    {
        printf(
            "ADC channel %d mean filter convert data is %d.\r\n",
            adc_channel_index + 1,
            adc_mean_filter_convert(adc_channel_list[adc_channel_index], 10));
    }

    for (pwm_test_duty = 0; pwm_test_duty <= PWM_DUTY_MAX / 2; pwm_test_duty++)
    {

        for (pwm_test_channel_index = 0; pwm_test_channel_index < PWM_CHANNEL_NUMBER; pwm_test_channel_index++)
        {
            pwm_test_duty_temp = (pwm_test_duty + pwm_test_channel_index * PWM_DUTY_MAX / 8) % (PWM_DUTY_MAX / 2) + (PWM_DUTY_MAX / 2);
            pwm_set_duty(pwm_channel_list[pwm_test_channel_index], pwm_test_duty_temp);
        }
        system_delay_us(200);
    }
}

void Sensor_Read()
{
    menc15a_data[0] = menc15a_get_absolute_data(menc15a_1_module);
    menc15a_data[1] = menc15a_get_absolute_data(menc15a_2_module);

    imu963ra_get_gyro();
    printf("dat:%d,%d,%d,%d,%d\n", menc15a_data[0], menc15a_data[1], imu963ra_gyro_x, imu963ra_gyro_y, imu963ra_gyro_z);
}
