/*
 * Uart.c
 *
 *  Created on: 2026��7��5��
 *      Author: XYY
 */
#include "Uart.h"
#include "Controller.h"



void Uart_init(void)
{
    uart_set_buffer(uart0_handle);
    uart_init(UART_0, 1000000, UART0_TX_P14_0, UART0_RX_P14_1);
    uart_rx_interrupt(UART_0, 1);
}



void Get_Vel(void)
{
    unsigned char byte;
    unsigned char *pEnd;
    unsigned char dir;
    while (uart_query_byte(UART_0, &byte)) {
        if (byte == '\n')
        {
            break;
        }
        
        switch (rx_state) {
            case 0:
                if (byte == 's') { rx_cnt = 0; rx_state = 1; }
                break;
            case 1:
                if (byte == 's')
                {
                    rx_state = 0;
                    break;
                }   
                if (byte != 'l' && byte != 'r')
                {
                    rx_buf[rx_cnt++] = byte;
                }
                else if (byte == 'l' || byte == 'r') 
                {
                    
                    dir = byte;
                    rx_state = 2;
                }
                break;
            case 2:

                if (byte == 's') {
                    float speed;
                    speed = strtof(rx_buf,&pEnd);
                    if (dir == 'l')
                    {
                        Controller[1].input_velocity_buffer = speed;
                        CONTROLLER_sync_callback(&Controller[1]);
                    }
                    else if (dir == 'r')
                    {
                        Controller[0].input_velocity_buffer = speed;
                        CONTROLLER_sync_callback(&Controller[0]);
                    }
                }
                else 
                {
                    rx_state = 0;
                    break;
                }
                rx_state = 0;
                break;
        }
    }
    

}

