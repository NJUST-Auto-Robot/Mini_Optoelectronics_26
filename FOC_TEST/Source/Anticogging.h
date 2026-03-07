
#include "Controller.h"
#include "FOC.h"
#include "Heap.h"
#include "FSM.h"
#include "PWM.h"
#include "User_config.h"
#include "util.h"
#include "stdbool.h"

extern bool AnticoggingValid;

void ANTICOGGING_start(void);
void ANTICOGGING_end(void);
void ANTICOGGING_loop(void);