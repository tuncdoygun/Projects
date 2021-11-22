#ifndef _IUART_H
#define _IUART_H

#define IUART_ST        USART2
#define IUART           UART_2
#define IUART_IRQn      USART2_IRQn

void IUART_Init(void);
void IUART_SendData(const void *buf, int len);

void Task_IUART(void);


#endif