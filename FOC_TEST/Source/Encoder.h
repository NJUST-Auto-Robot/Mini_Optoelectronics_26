

#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "main.h"
#include "PWM.h"
#include "User_config.h"
#include "util.h"
#include "spi.h"
#include "math.h"

#define ENCODER_CPR     (int) 16384
#define ENCODER_CPR_F   (16384.0f)
#define ENCODER_CPR_DIV (ENCODER_CPR >> 1)

#define ENC_NCS_RESET() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)
#define ENC_NCS_SET()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)

typedef struct sEncoder
{
    uint8_t need_init;

    int raw;
    int count_in_cpr;
    int count_in_cpr_prev;

    int64_t shadow_count;

    float pos;
    float vel;
    float phase;
    float phase_vel;

    // pll use
    float pll_pos;
    float pll_vel;

    float pll_kp;
    float pll_ki;
    float interpolation;
    float snap_threshold;
} tEncoder;

extern tEncoder Encoder;
extern uint16_t data;

void ENCODER_init(void);
void ENCODER_loop(void);

#endif
