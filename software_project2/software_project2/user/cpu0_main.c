#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"
#include "Anticogging.h"
#include "Controller.h"
#include "FSM.h"
// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?#pragma section all restoreï¿½ï¿½ï¿½Ö?ï¿½ï¿½ï¿½È?ï¿½Ö±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½CPU0ï¿½ï¿½RAMï¿½ï¿½

// **************************** ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ****************************

uint16 data_ = 0;
int finish = 0;
extern tController Controller[2];
extern volatile tFSM mFSM;

int core0_main(void)
{
    disable_Watchdog(); 
    clock_init();                   // ï¿½ï¿½È¡Ê±ï¿½ï¿½Æµï¿½ï¿½<ï¿½ï¿½Ø±ï¿½ï¿½ï¿?>
    debug_init();                   // ï¿½ï¿½Ê¼ï¿½ï¿½Ä¬ï¿½Ïµï¿½ï¿½Ô´ï¿½ï¿½ï¿½
    // ï¿½Ë´ï¿½ï¿½ï¿½Ð´ï¿½Ã»ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½

    gpio_init(P33_7,GPO,GPIO_HIGH,GPO_PUSH_PULL);
    gpio_init(P33_8,GPO,GPIO_LOW,GPO_PUSH_PULL);

    system_delay_init();
    Uart_init();

    USR_set_default_config();

    MCT_init();
    FOC_init();
    CONTROLLER_init();

    // if (USR_read_config()) {

    // }



    // if (0 == USR_read_cogging_map()) {
    //     AnticoggingValid = true;
    // } else {
    //     USR_set_default_cogging_map();
    // }

    PWMC_init();
    ENCODER_init();

    // ENCODER_loop(&Encoder[1]);    // ÏÈ¶ÁÒ»´ÎÈÃ need_init ¼õµ½ 19
    // ENCODER_loop(&Encoder[1]);    // ÔÙ¶à¶Á¼¸´ÎÈ·±£³õÊ¼»¯Íê³É
    // ENCODER_loop(&Encoder[1]);
    
    // if (PWMC_CurrentReadingPolarization() != 0) {
    //     StatuswordNew.errors.selftest = 1;
    // }



    MCT_set_state(IDLE);
//     mFSM.state = mFSM.state_next;
//     while (MCT_get_state() != IDLE) {
//         MCT_high_frequency_task(1);
//         MCT_low_priority_task();
//         system_delay_ms(1);
//     }
// // Ð£×¼ï¿½ï¿½É£ï¿½encoder_offset ï¿½Ñ¾ï¿½ï¿½ï¿½ UsrConfig ï¿½ï¿½ï¿½ï¿½
//     printf("calib done, offset=%d, dir=%d, pp=%d\n",
//         UsrConfig.encoder_offset,
//         UsrConfig.encoder_dir,
//         UsrConfig.motor_pole_pairs);

    UsrConfig.calib_valid = 1;
    MCT_set_state(IDLE);
    pit_init(CCU60_CH0, 50);

    system_delay_ms(20);

    CONTROLLER_set_op_mode(&Controller[0], CONTROL_MODE_VELOCITY_RAMP);
    CONTROLLER_set_op_mode(&Controller[1], CONTROL_MODE_VELOCITY_RAMP);  
    // // ï¿½Ë´ï¿½ï¿½ï¿½Ð´ï¿½Ã»ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    cpu_wait_event_ready();         // ï¿½È´ï¿½ï¿½ï¿½ï¿½Ðºï¿½ï¿½Ä³ï¿½Ê¼ï¿½ï¿½ï¿½ï¿½ï¿?


    while (TRUE)
    {

        if(gpio_get_level(P33_7) == 0)
            gpio_set_level(P33_7,1);
        else
            gpio_set_level(P33_7,0);
        // ï¿½Ë´ï¿½ï¿½ï¿½Ð´ï¿½ï¿½ÒªÑ­ï¿½ï¿½Ö´ï¿½ÐµÄ´ï¿½ï¿½ï¿½
        if (MCT_get_state() == IDLE) {
                MCT_set_state(RUN);     // ï¿½ï¿½Ç°ï¿½ï¿½ï¿½ï¿½ IDLEï¿½ï¿½RUN ï¿½á±»ï¿½ï¿½ï¿½ï¿½
                system_delay_ms(50); 
                if (MCT_get_state() != RUN) {
                    system_delay_ms(10);  // ï¿½ï¿½ ISR ï¿½Ðµï¿½ RUN
                }
            }
        
        // while (MCT_get_state() != IDLE) {
        //     MCT_low_priority_task();
        //     // system_delay_ms(1);
        // }
        // if (MCT_get_state() == IDLE && finish == 1) {
        //         printf("calib done, offset=%d, dir=%d, pp=%d\n",
        //     UsrConfig.encoder_offset,
        //     UsrConfig.encoder_dir,
        //     UsrConfig.motor_pole_pairs);
        //     }
        MCT_low_priority_task();
        // if(Controller.input_velocity_buffer >= 3.0f)
        //     Controller.input_velocity_buffer = Controller.input_velocity_buffer-0.5f;
        // if (Controller[1].input_velocity_buffer >= 2)
        // Controller[1].input_velocity_buffer -= 0.5;
        CONTROLLER_sync_callback(&Controller[1]);
        system_delay_ms(1000);

        // ï¿½Ë´ï¿½ï¿½ï¿½Ð´ï¿½ï¿½ÒªÑ­ï¿½ï¿½Ö´ï¿½ÐµÄ´ï¿½ï¿½ï¿½
    }
}

#pragma section all restore
// **************************** ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ****************************
