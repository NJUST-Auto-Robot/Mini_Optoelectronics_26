/*
 * self->c
 *
 *  Created on: 2026��3��19��
 *      Author: XYY
 */

#include "Controller.h"
#include "IfxCpu_Intrinsics.h"

tController Controller[2];

int CONTROLLER_set_op_mode(tController *self, tControlMode mode)
{
    __disable();

    self->ctrl_mode = mode;
    CONTROLLER_reset(self);

    __enable();

    return 0;
}

int CONTROLLER_set_home(tController *self)
{
    int ret = 0;

    if (MCT_get_state() == IDLE) {
        Encoder[self->motor_id].shadow_count = 0;
    } else if (MCT_get_state() == RUN) {
        if (ABS(Encoder[self->motor_id].vel) < 0.5f && Traj[self->motor_id].profile_done) {
            __disable();
            self->input_position        = 0;
            self->input_position_buffer = 0;
            self->pos_setpoint          = 0;
            Encoder[self->motor_id].shadow_count             = 0;
            Encoder[self->motor_id].pos                      = 0;
            __enable();
        } else {
            ret = -1;
        }
    } else {
        ret = -1;
    }

    return ret;
}

void CONTROLLER_sync_callback(tController *self)
{
    if (MCT_get_state() != RUN) {
        return;
    }

    switch (self->ctrl_mode) {
    case CONTROL_MODE_CURRENT_RAMP:
        self->input_current = self->input_current_buffer;
        break;

    case CONTROL_MODE_VELOCITY_RAMP:
        self->input_velocity = self->input_velocity_buffer;
        break;

    case CONTROL_MODE_POSITION_FILTER:
        self->input_position = self->input_position_buffer;
        break;

    case CONTROL_MODE_POSITION_PROFILE:
        self->input_position = self->input_position_buffer;
        self->input_updated  = true;
        break;

    default:
        break;
    }

    StatuswordNew.status.target_reached = 0;
}

void CONTROLLER_init(void)
{
    Controller[0].ctrl_mode = (tControlMode) UsrConfig.default_op_mode;
    Controller[1].ctrl_mode = (tControlMode) UsrConfig.default_op_mode;
    Controller[0].motor_id = 0;
    Controller[1].motor_id = 1;

    CONTROLLER_update_input_pos_filter_gain(&Controller[0], UsrConfig.position_filter_bw);
    CONTROLLER_update_input_pos_filter_gain(&Controller[1], UsrConfig.position_filter_bw);
}

void CONTROLLER_update_input_pos_filter_gain(tController *self, float bw)
{
    float bandwidth                = bw * M_2PI;
    self->input_pos_filter_ki = 2.0f * bandwidth;
    self->input_pos_filter_kp = 0.25f * SQ(self->input_pos_filter_ki);
}

void CONTROLLER_reset(tController *self)
{
    __disable();

    float pos_meas = Encoder[self->motor_id].pos;

    if (MCT_get_state() == ANTICOGGING) {
        pos_meas = (Encoder[self->motor_id].count_in_cpr / ENCODER_CPR_F);
    }

    self->input_position = pos_meas;
    self->input_velocity = 0.0f;
    self->input_current  = 0.0f;

    self->input_position_buffer = pos_meas;
    self->input_velocity_buffer = 0.0f;
    self->input_current_buffer  = 0.0f;

    self->pos_setpoint = pos_meas;
    self->vel_setpoint = 0.0f;
    self->cur_setpoint = 0.0f;

    self->vel_integrator = 0;

    self->input_updated = false;
    Traj[self->motor_id].profile_done        = true;

    Foc[self->motor_id].current_ctrl_integral_d = 0;
    Foc[self->motor_id].current_ctrl_integral_q = 0;

    __enable();
}

void CONTROLLER_loop(int id)
{
    float       vel_des;
    const float pos_meas       = Encoder[id].pos;
    const float vel_meas       = Encoder[id].vel;
    const float phase_meas     = Encoder[id].phase;
    const float phase_vel_meas = Encoder[id].phase_vel;

    if (MCT_get_state() == RUN) {
        switch (Controller[id].ctrl_mode) {
        // Current ramp
        case CONTROL_MODE_CURRENT_RAMP: {
            float max_step_size = ABS(CURRENT_MEASURE_PERIOD * UsrConfig.current_ramp_rate);
            float full_step     = Controller[id].input_current - Controller[id].cur_setpoint;
            float step          = CLAMP(full_step, -max_step_size, max_step_size);
            Controller[id].cur_setpoint += step;
        } break;

        // Velocity ramp
        case CONTROL_MODE_VELOCITY_RAMP: {
            float max_step_size = ABS(CURRENT_MEASURE_PERIOD * UsrConfig.velocity_ramp_rate);
            float full_step     = Controller[id].input_velocity - Controller[id].vel_setpoint;
            float step          = CLAMP(full_step, -max_step_size, max_step_size);
            Controller[id].vel_setpoint += step;
            Controller[id].cur_setpoint = UsrConfig.current_ff_gain * (step / CURRENT_MEASURE_PERIOD);

            // Target reached check
            if (StatuswordNew.status.target_reached == 0) {
                if (ABS(Controller[id].input_velocity - vel_meas) < UsrConfig.target_velcity_window) {
                    StatuswordNew.status.target_reached = 1;
                }
            }
        } break;

        // Position filter
        case CONTROL_MODE_POSITION_FILTER: {
            // 2nd order pos tracking filter
            float delta_pos = Controller[id].input_position - Controller[id].pos_setpoint;
            float delta_vel = Controller[id].input_velocity - Controller[id].vel_setpoint; // Vel error
            float accel     = Controller[id].input_pos_filter_kp * delta_pos
                          + Controller[id].input_pos_filter_ki * delta_vel;                  // Feedback
            Controller[id].cur_setpoint = UsrConfig.current_ff_gain * accel;                 // Accel
            Controller[id].vel_setpoint += CURRENT_MEASURE_PERIOD * accel;                   // Delta vel
            Controller[id].pos_setpoint += CURRENT_MEASURE_PERIOD * Controller[id].vel_setpoint; // Delta pos
        } break;

        // Position profile
        case CONTROL_MODE_POSITION_PROFILE: {
            if (Controller[id].input_updated) {
                Controller[id].input_updated = false;

                TRAJ_plan(&Traj[id], Controller[id].input_position,
                          Controller[id].pos_setpoint,
                          Controller[id].vel_setpoint,
                          UsrConfig.profile_velocity, // Velocity
                          UsrConfig.profile_accel,    // Acceleration
                          UsrConfig.profile_decel);   // Deceleration
            }

            if (Traj[id].profile_done) {
                // Target reached check
                if (StatuswordNew.status.target_reached == 0) {
                    if (ABS(Controller[id].input_position - pos_meas) < UsrConfig.target_position_window) {
                        StatuswordNew.status.target_reached = 1;
                    }
                }
                break;
            }

            TRAJ_eval(&Traj[id]);
            Controller[id].pos_setpoint = Traj[id].Y;
            Controller[id].vel_setpoint = Traj[id].Yd;
            Controller[id].cur_setpoint = Traj[id].Ydd * UsrConfig.current_ff_gain;
        } break;

        default:
            break;
        }

        // Position control
        vel_des = Controller[id].vel_setpoint;
        if (Controller[id].ctrl_mode >= CONTROL_MODE_POSITION_FILTER) {
            float pos_err = Controller[id].pos_setpoint - pos_meas;
            vel_des += UsrConfig.pos_p_gain * pos_err;
        }
    } else {
        // Anticogging calib
        float pos_err = Controller[id].input_position - (Encoder[id].count_in_cpr / ENCODER_CPR_F);
        if (pos_err > +0.5f){
            pos_err -= 1.0f;
        }
        if (pos_err < -0.5f){
            pos_err += 1.0f;
        }
        vel_des = UsrConfig.pos_p_gain * pos_err;
    }

    // Velocity limiting
    vel_des = CLAMP(vel_des, -UsrConfig.velocity_limit, +UsrConfig.velocity_limit);

    // Velocity control
    float iq_set = Controller[id].cur_setpoint;
    float v_err = 0.0f;
    if (Controller[id].ctrl_mode >= CONTROL_MODE_VELOCITY_RAMP) {
        v_err = vel_des - vel_meas;
        iq_set += UsrConfig.vel_p_gain * v_err;

        // Velocity integral action before limiting
        iq_set += Controller[id].vel_integrator;
    }

    // Velocity limiting in current mode
    if (Controller[id].ctrl_mode < CONTROL_MODE_VELOCITY_RAMP) {
        float Imax = (+UsrConfig.velocity_limit - vel_meas) * UsrConfig.vel_p_gain;
        float Imin = (-UsrConfig.velocity_limit - vel_meas) * UsrConfig.vel_p_gain;
        iq_set     = CLAMP(iq_set, Imin, Imax);
    }

    // Anticogging
    if (UsrConfig.anticogging_enable && AnticoggingValid) {
        int16_t index = nearbyintf(COGGING_MAP_NUM * Encoder[id].count_in_cpr / ENCODER_CPR_F);
        if (index >= COGGING_MAP_NUM) {
            index = 0;
        }
        iq_set += pCoggingMap->map[index] / 5000.0f;
    }

    // Current limit
    bool limited = false;
    if (iq_set > +UsrConfig.current_limit) {
        limited = true;
        iq_set  = +UsrConfig.current_limit;
    }
    if (iq_set < -UsrConfig.current_limit) {
        limited = true;
        iq_set  = -UsrConfig.current_limit;
    }

    FOC_current(id, 0, iq_set, phase_meas, phase_vel_meas);

    // Velocity integrator
    if (Controller[id].ctrl_mode < CONTROL_MODE_VELOCITY_RAMP) {
        // reset integral if not in use
        Controller[id].vel_integrator = 0.0f;
    } else {
        if (limited) {
            Controller[id].vel_integrator *= 0.99f;
        } else {
            Controller[id].vel_integrator += (UsrConfig.vel_i_gain * CURRENT_MEASURE_PERIOD) * v_err;
        }
    }
}
