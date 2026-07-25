/*
 * self->c
 *
 *  Created on: 2026��3��19��
 *      Author: XYY
 */

#include "Encoder.h"

tEncoder Encoder[2];
uint16_t data;
void ENCODER_init(void)
{
    // Init
    for (int i = 0; i < 2; i++)
    {
        Encoder[i].need_init    = 20;
        Encoder[i].shadow_count = 0;
        Encoder[i].pll_pos      = 0;
        Encoder[i].pll_vel      = 0;

        int encoder_pll_bw     = 100 * M_2PI;
        Encoder[i].pll_kp         = 2.0f * encoder_pll_bw;      // basic conversion to discrete time
        Encoder[i].pll_ki         = 0.25f * SQ(Encoder[i].pll_kp); // Critically damped
        Encoder[i].snap_threshold = 1.0f * CURRENT_MEASURE_PERIOD * Encoder[i].pll_ki;
    }


    menc15a_init();
    
    
}

uint16_t Parity_bit_Calculate(uint16_t data_2_cal)
{
    uint16_t parity_bit_value=0;
    while(data_2_cal != 0)
    {
        parity_bit_value ^= data_2_cal;
        data_2_cal >>=1;
    }
    return (parity_bit_value & 0x1);
}

uint16_t SPI_ReadWrite_OneByte(uint16_t _txdata)
{

    ENC_NCS_RESET();
    uint16_t rxdata;
    spi_transfer_8bit(SPI_1,(uint8_t *)&_txdata,(uint8_t *)&rxdata,1);
//    while (RESET==__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE))
//    {
//      __nop();
//    }
    ENC_NCS_SET();
    return rxdata;
}
uint16_t AS5047_read(uint16_t add)
{
    add |= 0x4000;
    if(Parity_bit_Calculate(add)==1){
        add=add|0x8000;
    }
    SPI_ReadWrite_OneByte(add);
    __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
    __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
        data=SPI_ReadWrite_OneByte(0x0000|0x4000);
    data &=0x3fff;
    return data;
}
float delta_pos;
void ENCODER_loop(tEncoder *self)
{
    //self->raw = AS5047_read(0x3FFF);
    if (self->motor_id == 0)
    {
        self->raw = menc15a_get_absolute_data(menc15a_1_module);
    }
    else
    {
        self->raw = menc15a_get_absolute_data(menc15a_2_module);
    }
    if (UsrConfig.encoder_dir == -1) {
        self->raw = ENCODER_CPR - 1 - self->raw;
    }

    /* Linearization */
    int off_1      = UsrConfig.offset_lut[(self->raw) >> 7];
    int off_2      = UsrConfig.offset_lut[((self->raw >> 7) + 1) % 128];
    int off_interp = off_1 + ((off_2 - off_1) * (self->raw - ((self->raw >> 7) << 7)) >> 7);

    // int count = self->raw - off_interp - UsrConfig.encoder_offset;
    int count = self->raw - UsrConfig.encoder_offset;

    /*  Wrap in ENCODER_CPR */
    while (count > ENCODER_CPR)
        count -= ENCODER_CPR;
    while (count < 0)
        count += ENCODER_CPR;

    self->count_in_cpr = count;

    if (self->need_init) {
        self->need_init--;
        self->count_in_cpr_prev = self->count_in_cpr;
        return;
    }

    /* Delta count */
    int delta_count           = self->count_in_cpr - self->count_in_cpr_prev;
    self->count_in_cpr_prev = self->count_in_cpr;
    while (delta_count > +ENCODER_CPR_DIV)
        delta_count -= ENCODER_CPR;
    while (delta_count < -ENCODER_CPR_DIV)
        delta_count += ENCODER_CPR;

    // Run pll (for now pll is in units of encoder counts)
    // Predict current pos
    self->pll_pos += CURRENT_MEASURE_PERIOD * self->pll_vel;
    // Discrete phase detector
    delta_pos = self->count_in_cpr - floorf(self->pll_pos);
    // delta_pos = 0.0f;
    while (delta_pos > +ENCODER_CPR_DIV){
        delta_pos -= ENCODER_CPR_F;
    }
    while (delta_pos < -ENCODER_CPR_DIV){
        delta_pos += ENCODER_CPR_F;
    }
    // PLL feedback
    self->pll_pos += CURRENT_MEASURE_PERIOD * self->pll_kp * delta_pos;
    while (self->pll_pos > ENCODER_CPR){
        self->pll_pos -= ENCODER_CPR_F;
    }
    while (self->pll_pos < 0){
        self->pll_pos += ENCODER_CPR_F;
    }
    self->pll_vel += CURRENT_MEASURE_PERIOD * self->pll_ki * delta_pos;

    // Align delta-sigma on zero to prevent jitter
    if (ABS(self->pll_vel) < self->snap_threshold) {
        self->pll_vel = 0.0f;
    }

    /* Outputs from Encoder for Controller */
    self->shadow_count += delta_count;
    self->pos       = self->shadow_count / ENCODER_CPR_F;
    self->vel       = self->pll_vel / ENCODER_CPR_F;
    self->phase     = (M_2PI * UsrConfig.motor_pole_pairs) * self->count_in_cpr / ENCODER_CPR_F;
    self->phase_vel = (M_2PI * UsrConfig.motor_pole_pairs) * self->vel;
}
