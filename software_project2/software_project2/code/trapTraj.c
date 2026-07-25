/*
 * trapself->c
 *
 *  Created on: 2026��3��19��
 *      Author: XYY
 */

#include "trapTraj.h"

tTraj Traj[2];

// A sign function where input 0 has positive sign (not 0)
static inline float sign_hard(float val)
{
    return (signbit(val)) ? -1.0f : 1.0f;
}

void TRAJ_plan(tTraj *self, float position, float start_position, float start_velocity, float Vmax, float Amax, float Dmax)
{
    float distance  = position - start_position;           // Distance to travel
    float stop_dist = SQ(start_velocity) / (2.0f * Dmax);  // Minimum stopping distance
    float dXstop    = copysign(stop_dist, start_velocity); // Minimum stopping displacement
    float s         = sign_hard(distance - dXstop);        // Sign of coast velocity (if any)
    self->acc        = s * Amax;                            // Maximum Acceleration (signed)
    self->dec        = -s * Dmax;                           // Maximum Deceleration (signed)
    self->vel        = s * Vmax;                            // Maximum Velocity (signed)

    // If we start with a speed faster than cruising, then we need to decel instead of accel aka "double deceleration move" in the paper
    if ((s * start_velocity) > (s * self->vel)) {
        self->acc = -s * Amax;
    }

    // Time to accel/decel to/from Vr (cruise speed)
    self->t_acc = (self->vel - start_velocity) / self->acc;
    self->t_dec = -self->vel / self->dec;

    // Integral of velocity ramps over the full accel and decel times to get
    // minimum displacement required to reach cuising speed
    float dXmin = 0.5f * self->t_acc * (self->vel + start_velocity) + 0.5f * self->t_dec * self->vel;

    // Are we displacing enough to reach cruising speed?
    if (s * distance < s * dXmin) {
        // Short move (triangle profile)
        self->vel = s
                   * sqrtf(fmax((self->dec * SQ(start_velocity) + 2.0f * self->acc * self->dec * distance)
                                    / (self->dec - self->acc),
                                0.0f));
        self->t_acc = fmax(0.0f, (self->vel - start_velocity) / self->acc);
        self->t_dec = fmax(0.0f, -self->vel / self->dec);
        self->t_vel = 0.0f;
    } else {
        // Long move (trapezoidal profile)
        self->t_vel = (distance - dXmin) / self->vel;
    }

    // Fill in the rest of the values used at evaluation-time
    self->t_total        = self->t_acc + self->t_vel + self->t_dec;
    self->start_position = start_position;
    self->start_velocity = start_velocity;
    self->end_position   = position;
    self->acc_distance   = start_position + start_velocity * self->t_acc
                        + 0.5f * self->acc * SQ(self->t_acc); // pos at end of accel phase

    self->tick         = 0;
    self->profile_done = false;
}

void TRAJ_eval(tTraj *self)
{
    if (self->profile_done) {
        return;
    }

    self->tick++;
    float t = self->tick * CURRENT_MEASURE_PERIOD;

    if (t < 0.0f) { // Initial Condition
        self->Y   = self->start_position;
        self->Yd  = self->start_velocity;
        self->Ydd = 0.0f;
    } else if (t < self->t_acc) { // Accelerating
        self->Y   = self->start_position + self->start_velocity * t + 0.5f * self->acc * SQ(t);
        self->Yd  = self->start_velocity + self->acc * t;
        self->Ydd = self->acc;
    } else if (t < self->t_acc + self->t_vel) { // Coasting
        self->Y   = self->acc_distance + self->vel * (t - self->t_acc);
        self->Yd  = self->vel;
        self->Ydd = 0.0f;
    } else if (t < self->t_total) { // Deceleration
        float td = t - self->t_total;
        self->Y   = self->end_position + 0.5f * self->dec * SQ(td);
        self->Yd  = self->dec * td;
        self->Ydd = self->dec;
    } else if (t >= self->t_total) { // Final Condition
        self->Y            = self->end_position;
        self->Yd           = 0.0f;
        self->Ydd          = 0.0f;
        self->profile_done = true;
    }
}
