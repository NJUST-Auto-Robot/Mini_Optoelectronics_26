/*
 * User_Config.h
 *
 *  Created on: 2026年3月13日
 *      Author: XYY
 */

#ifndef USER_USER_CONFIG_H_
#define USER_USER_CONFIG_H_

#include "zf_common_headfile.h"

#define OFFSET_LUT_NUM   128U
#define COGGING_MAP_NUM  5000U
//#define FLASH_PAGE_SIZE                 0x800U  /* 2 KB */
//#define FLASH_BASE            (0x08000000UL) /*!< FLASH (up to 128 kB) base address */

typedef struct sUsrConfig{
    // Motor
    uint32_t invert_motor_dir;       // 0 False : 1 True
    uint32_t motor_pole_pairs;       // [PP]      (2~30)
    float   motor_phase_resistance; // [R]       (0~10)
    float   motor_phase_inductance; // [H]       (0~1)
    float   current_limit;          // [A]       (0~10)
    float   velocity_limit;         // [r/s]     (0~100)
    // Calibration
    float calib_current; // [A] (0~10)
    float calib_voltage; // [V] (0~50)

    // Controller
    float   pos_p_gain;
    float   vel_p_gain;
    float   vel_i_gain;
    float   current_ff_gain; // [A/(r/s²)]
    float   current_ctrl_bw; // [Hz] (100~2000)
    uint32_t default_op_mode;
    uint32_t anticogging_enable;     // 0 False : 1 True
    uint32_t sync_target_enable;     // 0 False : 1 True
    float   target_velcity_window;  // [r/s]
    float   target_position_window; // [r]
    float   current_ramp_rate;      // [A/s]
    float   velocity_ramp_rate;     // [r/s²]
    float   position_filter_bw;     // [Hz] (0~1000)
    float   profile_velocity;       // [r/s]
    float   profile_accel;          // [r/s²]
    float   profile_decel;          // [r/s²]
    // Encoder
    int32_t calib_valid;                // (Auto)
    int32_t encoder_dir;                // (Auto)
    int32_t encoder_offset;             // (Auto)
    int32_t offset_lut[OFFSET_LUT_NUM]; // (Auto)
    uint32_t crc;
}tUsrConfig;

typedef struct sCoggingMap
{
    int16_t  map[COGGING_MAP_NUM];
    uint32_t crc;
} tCoggingMap;

extern tUsrConfig   UsrConfig;
extern tCoggingMap *pCoggingMap;

void USR_set_default_config(void);
void USR_set_default_cogging_map(void);
int  USR_read_config(void);
int  USR_save_config(void);
int  USR_read_cogging_map(void);
int  USR_save_cogging_map(void);
int  USR_erease_config(void);
int  USR_erease_cogging_map(void);

#endif /* USER_USER_CONFIG_H_ */
