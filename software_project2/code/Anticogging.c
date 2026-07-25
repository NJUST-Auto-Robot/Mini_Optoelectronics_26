/*
 * Anticogging.c
 *
 *  Created on: 2026��3��19��
 *      Author: XYY
 */

#include "Anticogging.h"
#include "controller.h"
#include "foc.h"
#include "User_config.h"
#include "FSM.h"

bool AnticoggingValid = false;

static int mNumber;
static int mLoopCount;

void ANTICOGGING_start(void)
{
    mNumber          = 0;
    mLoopCount       = 0;
    AnticoggingValid = false;
    USR_set_default_cogging_map();
}

void ANTICOGGING_end(void)
{
    FOC_disarm(&Foc[0]);
    FOC_disarm(&Foc[1]);

    if (!AnticoggingValid) {
        USR_set_default_cogging_map();
    }
}

void ANTICOGGING_loop(int id)
{
 static const int gap_number = 100;

    // loop contrl 0.025s
    if (++mLoopCount < 500) {
        return;
    }
    mLoopCount = 0;

    mNumber++;

    if (mNumber <= gap_number) {
        // CW
        const float delta   = (1.0f / (float) COGGING_MAP_NUM);
        float       pos_ref = Controller[id].input_position + delta;
        if (pos_ref > 1.0f) {
            pos_ref -= 1.0f;
        }
        Controller[id].input_position = pos_ref;
    } else if (mNumber <= (gap_number + COGGING_MAP_NUM)) {
        int16_t tmp   = (int16_t) (Foc[id].i_q_filt * 5000.0f);
        int16_t index = nearbyintf(COGGING_MAP_NUM * Controller[id].input_position);
        if (index >= COGGING_MAP_NUM) {
            index = 0;
        }
        pCoggingMap->map[index] = tmp;


        // CW
        const float delta   = (1.0f / (float) COGGING_MAP_NUM);
        float       pos_ref = Controller[id].input_position + delta;
        if (pos_ref > 1.0f) {
            pos_ref -= 1.0f;
        }
        Controller[id].input_position = pos_ref;
    } else if (mNumber <= (gap_number + COGGING_MAP_NUM + gap_number)) {
        // CCW
        const float delta   = -(1.0f / (float) COGGING_MAP_NUM);
        float       pos_ref = Controller[id].input_position + delta;
        if (pos_ref < 0.0f) {
            pos_ref += 1.0f;
        }
        Controller[id].input_position = pos_ref;
    } else if (mNumber <= (gap_number + COGGING_MAP_NUM + gap_number + COGGING_MAP_NUM)) {
        int16_t tmp   = (int16_t) (Foc[id].i_q_filt * 5000.0f);
        int16_t index = nearbyintf(COGGING_MAP_NUM * Controller[id].input_position);
        if (index >= COGGING_MAP_NUM) {
            index = 0;
        }
        pCoggingMap->map[index] = (pCoggingMap->map[index] + tmp) / 2;


        // CCW
        const float delta   = -(1.0f / (float) COGGING_MAP_NUM);
        float       pos_ref = Controller[id].input_position + delta;
        if (pos_ref < 0.0f) {
            pos_ref += 1.0f;
        }
        Controller[id].input_position = pos_ref;
    } else {
        // End
        AnticoggingValid = true;
        MCT_set_state(IDLE);
    }
}
