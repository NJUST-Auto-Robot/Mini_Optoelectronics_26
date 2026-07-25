/*
 * Uart.h
 *
 *  Created on: 2026��7��5��
 *      Author: XYY
 */

#ifndef CODE_UART_H_
#define CODE_UART_H_
#include "zf_common_headfile.h"

#define FRAME_HEADER  0xAA
#define FRAME_FOOTER  0x55
static unsigned char rx_buf[10];
static uint8 rx_cnt = 0;
static uint8 rx_state = 0;


void Uart_init(void);
void Get_Vel(void);

#endif /* CODE_UART_H_ */
