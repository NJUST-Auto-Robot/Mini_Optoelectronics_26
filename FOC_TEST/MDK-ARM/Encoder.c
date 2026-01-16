
#include "Encoder.h"

tEncoder Encoder;
uint16_t data;
void ENCODER_init(void)
{
    // Init
    Encoder.need_init    = 20;
    Encoder.shadow_count = 0;
    Encoder.pll_pos      = 0;
    Encoder.pll_vel      = 0;

    int encoder_pll_bw     = 100 * M_2PI;
    Encoder.pll_kp         = 2.0f * encoder_pll_bw;      // basic conversion to discrete time
    Encoder.pll_ki         = 0.25f * SQ(Encoder.pll_kp); // Critically damped
    Encoder.snap_threshold = 1.0f * CURRENT_MEASURE_PERIOD * Encoder.pll_ki;
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
	HAL_SPI_TransmitReceive(&hspi1,(uint8_t *)&_txdata,(uint8_t *)&rxdata,1,HAL_MAX_DELAY);
    while (RESET==__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_TXE))
    {
      __nop();  
    }
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
void ENCODER_loop(void)
{
    Encoder.raw = AS5047_read(0x3FFF);
    if (UsrConfig.encoder_dir == -1) {
        Encoder.raw = ENCODER_CPR - 1 - Encoder.raw;
    }

    /* Linearization */
    int off_1      = UsrConfig.offset_lut[(Encoder.raw) >> 7];
    int off_2      = UsrConfig.offset_lut[((Encoder.raw >> 7) + 1) % 128];
    int off_interp = off_1 + ((off_2 - off_1) * (Encoder.raw - ((Encoder.raw >> 7) << 7)) >> 7);

    int count = Encoder.raw - off_interp - UsrConfig.encoder_offset;

    /*  Wrap in ENCODER_CPR */
    while (count > ENCODER_CPR)
        count -= ENCODER_CPR;
    while (count < 0)
        count += ENCODER_CPR;

    Encoder.count_in_cpr = count;

    if (Encoder.need_init) {
        Encoder.need_init--;
        Encoder.count_in_cpr_prev = Encoder.count_in_cpr;
        return;
    }

    /* Delta count */
    int delta_count           = Encoder.count_in_cpr - Encoder.count_in_cpr_prev;
    Encoder.count_in_cpr_prev = Encoder.count_in_cpr;
    while (delta_count > +ENCODER_CPR_DIV)
        delta_count -= ENCODER_CPR;
    while (delta_count < -ENCODER_CPR_DIV)
        delta_count += ENCODER_CPR;

    // Run pll (for now pll is in units of encoder counts)
    // Predict current pos
    Encoder.pll_pos += CURRENT_MEASURE_PERIOD * Encoder.pll_vel;
    // Discrete phase detector
    delta_pos = Encoder.count_in_cpr - floorf(Encoder.pll_pos);
    while (delta_pos > +ENCODER_CPR_DIV){
        delta_pos -= ENCODER_CPR_F;
    }
    while (delta_pos < -ENCODER_CPR_DIV){
        delta_pos += ENCODER_CPR_F;
    }
    // PLL feedback
    Encoder.pll_pos += CURRENT_MEASURE_PERIOD * Encoder.pll_kp * delta_pos;
    while (Encoder.pll_pos > ENCODER_CPR){
        Encoder.pll_pos -= ENCODER_CPR_F;
    }
    while (Encoder.pll_pos < 0){
        Encoder.pll_pos += ENCODER_CPR_F;
    }
    Encoder.pll_vel += CURRENT_MEASURE_PERIOD * Encoder.pll_ki * delta_pos;

    // Align delta-sigma on zero to prevent jitter
    if (ABS(Encoder.pll_vel) < Encoder.snap_threshold) {
        Encoder.pll_vel = 0.0f;
    }

    /* Outputs from Encoder for Controller */
    Encoder.shadow_count += delta_count;
    Encoder.pos       = Encoder.shadow_count / ENCODER_CPR_F;
    Encoder.vel       = Encoder.pll_vel / ENCODER_CPR_F;
    Encoder.phase     = (M_2PI * UsrConfig.motor_pole_pairs) * Encoder.count_in_cpr / ENCODER_CPR_F;
    Encoder.phase_vel = (M_2PI * UsrConfig.motor_pole_pairs) * Encoder.vel;
}
