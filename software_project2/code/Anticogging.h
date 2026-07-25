/*
 * Anticogging.h
 *
 *  Created on: 2026��3��19��
 *      Author: XYY
 */

#ifndef USER_ANTICOGGING_H_
#define USER_ANTICOGGING_H_


#include "zf_common_headfile.h"

extern bool AnticoggingValid;

void ANTICOGGING_start(void);
void ANTICOGGING_end(void);
void ANTICOGGING_loop(int id);

#endif /* USER_ANTICOGGING_H_ */
