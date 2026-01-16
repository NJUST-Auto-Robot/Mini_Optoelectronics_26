

#include "FSM.h"


typedef struct sFSM
{
    tFSMState state;
    tFSMState state_next;
    uint8_t   state_next_ready;
} tFSM;

volatile tFSM mFSM;

volatile tMCStatusword StatuswordNew;
volatile tMCStatusword StatuswordOld;
;

#define CHARGE_BOOT_CAP_MS    10
#define CHARGE_BOOT_CAP_TICKS (uint16_t) ((PWM_FREQUENCY * CHARGE_BOOT_CAP_MS) / 1000)
static uint16_t mChargeBootCapDelay = 0;

static void enter_state(void);
static void exit_state(void);

void MCT_init(void)
{
    mFSM.state            = BOOT_UP;
    mFSM.state_next       = BOOT_UP;
    mFSM.state_next_ready = 0;

    StatuswordNew.status.status_code = 0;
    StatuswordNew.errors.errors_code = 0;
    StatuswordOld                    = StatuswordNew;
}

int MCT_reset_error(void)
{
    StatuswordNew.errors.errors_code &= 0x80;
    StatuswordOld.errors.errors_code &= 0x80;
    return 0;
}

tFSMState MCT_get_state(void)
{
    return mFSM.state;
}

// return
//    0 Success
//   -1 Invalid
//   -2 Error code
//   -3 Calib invalid
int MCT_set_state(tFSMState state)
{
    int ret = 0;

    switch (mFSM.state) {
    case BOOT_UP:
        if (state == IDLE) {
            mFSM.state_next = IDLE;
        } else {
            ret = -1;
        }
        break;

    case IDLE:
        switch (state) {
        case IDLE:
            FOC_disarm();
            mChargeBootCapDelay = 0;
            mFSM.state_next     = IDLE;
            break;

        case RUN:
            if (StatuswordNew.errors.errors_code) {
                ret = -2;
            } else if (!UsrConfig.calib_valid) {
                ret = -3;
            } else {
                FOC_arm();
                mChargeBootCapDelay = CHARGE_BOOT_CAP_TICKS;
                mFSM.state_next     = RUN;
            }
            break;

        case CALIBRATION:
            if (StatuswordNew.errors.errors_code) {
                ret = -2;
            } else {
                FOC_arm();
                mChargeBootCapDelay = CHARGE_BOOT_CAP_TICKS;
                mFSM.state_next     = CALIBRATION;
            }
            break;

        case ANTICOGGING:
            if (StatuswordNew.errors.errors_code) {
                ret = -2;
            } else if (!UsrConfig.calib_valid) {
                ret = -3;
            } else {
                FOC_arm();
                mChargeBootCapDelay = CHARGE_BOOT_CAP_TICKS;
                mFSM.state_next     = ANTICOGGING;
            }
            break;

        default:
            ret = -1;
            break;
        }
        break;

    default:
        if (state == IDLE) {
            mFSM.state_next = IDLE;
        } else {
            ret = -1;
        }
        break;
    }

    mFSM.state_next_ready = 0;

    return ret;
}

static void enter_state(void)
{
    switch (mFSM.state) {
    case BOOT_UP:
        break;

    case IDLE:
        break;

    case RUN:
        CONTROLLER_reset();
        StatuswordNew.status.switched_on    = 1;
        StatuswordNew.status.target_reached = 1;
        StatuswordOld.status                = StatuswordNew.status;
        break;

    case CALIBRATION:
        CALIBRATION_start();
        break;

    case ANTICOGGING:
        CONTROLLER_reset();
        USR_erease_cogging_map();
        __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
        __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
        ANTICOGGING_start();
        break;

    default:
        break;
    }
}

static void exit_state(void)
{
    switch (mFSM.state) {
    case BOOT_UP:
        mFSM.state_next_ready = 1;
        break;

    case IDLE:
        if (mChargeBootCapDelay) {
            mChargeBootCapDelay--;
        } else {
            mFSM.state_next_ready = 1;
        }
        break;

    case RUN:
        FOC_disarm();
        StatuswordNew.status.switched_on    = 0;
        StatuswordNew.status.target_reached = 0;
        StatuswordOld.status                = StatuswordNew.status;
        mFSM.state_next_ready               = 1;
        break;

    case CALIBRATION:
        CALIBRATION_end();
        mFSM.state_next_ready = 1;
        break;

    case ANTICOGGING:
        ANTICOGGING_end();
        mFSM.state_next_ready = 1;
        break;

    default:
        break;
    }
}
        __IO int tmp=0;
        __IO float val=I_SCALE;

void MCT_high_frequency_task(void)
{
    /* state transition management */
    if (mFSM.state_next != mFSM.state) {
        exit_state();
        if (mFSM.state_next_ready) {
            mFSM.state = mFSM.state_next;
            enter_state();
        }
    }

    ENCODER_loop();

    Foc.v_bus = read_vbus();
    UTILS_LP_FAST(Foc.v_bus_filt, Foc.v_bus, 0.05f);
    Foc.i_a = -read_iphase_a();
    Foc.i_b = -read_iphase_b();
    Foc.i_c = -(Foc.i_a + Foc.i_b);

    switch (mFSM.state) {
    case BOOT_UP:
        break;

    case CALIBRATION:
        CALIBRATION_loop();
        break;

    case ANTICOGGING:
        ANTICOGGING_loop();

    case RUN:
        CONTROLLER_loop();
        break;

    default:
        break;
    }
}


void MCT_low_priority_task(void)
{
    bool isSend = false;

    // State check
    if (StatuswordOld.status.status_code != StatuswordNew.status.status_code) {
        StatuswordOld.status.status_code = StatuswordNew.status.status_code;
    }

    // Error check
    if (StatuswordOld.errors.errors_code != StatuswordNew.errors.errors_code) {
        if (StatuswordNew.errors.errors_code) {
            FOC_disarm();
            MCT_set_state(IDLE);
        }
        isSend                           = true;
        StatuswordOld.errors.errors_code = StatuswordNew.errors.errors_code;
    }
}