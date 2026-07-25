/*
 * Encoder.h
 *
 *  Created on: 2026��3��19��
 *      Author: XYY
 */

#ifndef USER_ENCODER_H_
#define USER_ENCODER_H_

#include "all.h"
#include "User_config.h"
#include "util.h"
#include "math.h"

#define ENCODER_CPR     (int) 32768
#define ENCODER_CPR_F   (32768.0f)
#define ENCODER_CPR_DIV (ENCODER_CPR >> 1)

#define ENC_NCS_RESET() gpio_set_level(P20_9, 0);
#define ENC_NCS_SET()   gpio_set_level(P20_9, 1);

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

    uint8_t motor_id;
} tEncoder;

extern tEncoder Encoder[2];
extern uint16_t data;

void ENCODER_init(void);
void ENCODER_loop(tEncoder *self);

#endif /* USER_ENCODER_H_ */
