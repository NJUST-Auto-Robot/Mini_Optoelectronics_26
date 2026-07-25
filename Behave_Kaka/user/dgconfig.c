/*
 * dgconfig.c
 *
 *  Created on: 2026年7月25日
 *      Author: dragon
 */

#include "dgconfig.h"
#include "zf_common_headfile.h"

#define LED1                    (P33_7)
#define LED2                    (P33_8)

//PCB从上往下依次是KEY1,KEY2
#define KEY1                    (P33_12)
#define KEY2                    (P33_10)
#define KEY3                    (P33_11)
#define KEY4                    (P33_9)


//测试用临时变量
uint16 test_delay_time = 0;
uint8 led_state = 0;
void Base_Peripheral_Init(void)
{

    gpio_init(LED1, GPO, GPIO_LOW, GPO_PUSH_PULL);          // 初始化 LED1 输出 默认高电平 推挽输出模式
    gpio_init(LED2, GPO, GPIO_HIGH, GPO_PUSH_PULL);         // 初始化 LED2 输出 默认高电平 推挽输出模式

    gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY1 输入 默认高电平 上拉输入
    gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY2 输入 默认高电平 上拉输入
    gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY3 输入 默认高电平 上拉输入
    gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY4 输入 默认高电平 上拉输入

}
void Module_Peripheral_Init()
{



}

void led_key_test()
{

    test_delay_time = 300;
    if( !gpio_get_level(KEY1) || !gpio_get_level(KEY2) || !gpio_get_level(KEY3) || !gpio_get_level(KEY4) )         // 获取 KEYx 电平为低
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
