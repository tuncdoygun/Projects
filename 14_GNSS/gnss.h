#ifndef _GNSS_H
#define _GNSS_H

#define GUART_ST        USART1
#define GUART           UART_1
#define GUART_IRQn      USART1_IRQn

void GNSS_Init(void);
void Task_GNSS(void);


#endif
