#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "uart.h"
#include "iuart.h"
#include "fifo.h"

#define SZ_BUF          256

static unsigned char _RxBuf[SZ_BUF];
static unsigned char _TxBuf[SZ_BUF];

static FIFO     _RxFifo, _TxFifo;

void IUART_Init(void)
{
  // IUART baslangic
  UART_Init(IUART, 9600);
  
  // FIFO baslangic
  FIFO_Init(&_RxFifo, _RxBuf, SZ_BUF);
  FIFO_Init(&_TxFifo, _TxBuf, SZ_BUF);
  
  // Interrupt
  // a) ST yakasi
  // normalde false interrupt ile flag'i sifirliyorduk.Fakat burda gerek yok.
  // cunku burda flag bitleri donanimsal olarak kontrol ediliyor.
  USART_ITConfig(IUART_ST, USART_IT_RXNE, ENABLE);
  // USART_ITConfig(IUART_ST, USART_IT_TXE, ENABLE); // Kapali olmali.Cunku zaten TX buffer bos oldugundan direk interrupta duser.
  
  // b) ARM yakasi
  NVIC_SetPriority(IUART_IRQn, 3);
  NVIC_EnableIRQ(IUART_IRQn);
}

volatile static int _txFlag; // mevcut gonderme islemi var mi yok mu

void USART2_IRQHandler(void)
{
  unsigned char c;
  
  if (USART_GetITStatus(IUART_ST, USART_IT_RXNE)) {
    // gelen veriyi alýyoruz
    c = USART_ReceiveData(IUART_ST);
    
    // FIFO buffer'a ekliyoruz
    FIFO_SetData(&_RxFifo, c);
  }
  
  if (USART_GetITStatus(IUART_ST, USART_IT_TXE)) {
    // Tx FIFO'dan veri çekiyoruz
    c = FIFO_GetData(&_TxFifo);
    
    // Veriyi TDR'a yüklüyoruz
    USART_SendData(IUART_ST, c);
    
    // Gönderilecek veri kalmadýysa TX kesme kaynaðý kapatýlmalý
    if (FIFO_IsEmpty(&_TxFifo)) {
      USART_ITConfig(IUART_ST, USART_IT_TXE, DISABLE); 
      _txFlag = 0;
    }
  }  
}

void IUART_SendData(const void *buf, int len)
{
  const unsigned char *ptr = (const unsigned char *)buf;
  
  // Önce Tx FIFO'ya göndereceðimiz veriyi yüklüyoruz
  while (!FIFO_IsFull(&_TxFifo) && (len-- > 0))
    FIFO_SetData(&_TxFifo, *ptr++);
  
  _txFlag = 1;
  // Gönderme kesmesini açýyoruz
  USART_ITConfig(IUART_ST, USART_IT_TXE, ENABLE); 
}

void Task_IUART(void)
{
  char str[256];
  unsigned char lineBuf[256];
  int len;
  static unsigned long count;
  
  if(FIFO_GetLine(&_RxFifo, lineBuf))
    printf("%s", lineBuf); // line'in en sonunda \n var.
  
  if (!_txFlag) {
    len = sprintf(str, "Bu deneme kesmeli amacli gonderme islemi: %lu\r\n",
            ++count);
    IUART_SendData(str, len);
  }  
}