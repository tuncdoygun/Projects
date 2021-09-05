#ifndef _IO_H
#define _IO_H

#include "stm32f0xx_gpio.h"

enum {
  IO_PORT_A,
  IO_PORT_B,
  IO_PORT_C,
  IO_PORT_D,
  IO_PORT_E,
  IO_PORT_F,
  IO_PORT_G,
};

typedef struct {
  int mode;
  int otype;
  int pupd;
} IO_MODE;

typedef struct {
  int port;
  int pin;
} IO_PIN;

enum {
  IO_MODE_INPUT = GPIO_Mode_IN,
  IO_MODE_OUTPUT = GPIO_Mode_OUT,
  IO_MODE_AF = GPIO_Mode_AF,
  IO_MODE_ANALOG = GPIO_Mode_AN,
};

enum {
  IO_OTYPE_PP = GPIO_OType_PP,
  IO_OTYPE_OD = GPIO_OType_OD,
};

enum {
  IO_PUPD_NOPULL = GPIO_PuPd_NOPULL,
  IO_PUPD_UP = GPIO_PuPd_UP,
  IO_PUPD_DOWN = GPIO_PuPd_DOWN,
};

enum {
  IOP_LED,      // Bluepill LED (C13)
  //IOP_TEST,     // Test amaçlý(B5)
  //IOP_MYINPUT,  // Test amaçlý okuma A3
  
  // LCD
  IOP_LCD_RS,
  IOP_LCD_E,
  IOP_LCD_DB4,
  IOP_LCD_DB5,
  IOP_LCD_DB6,
  IOP_LCD_DB7,
  
  // UART
  IOP_U1RX,
  IOP_U1TX,
  IOP_U2RX,
  IOP_U2TX,
};

#ifdef _IOS_ // birkere tanýmlanmasý gereken

IO_PIN _ios[] = {
  {IO_PORT_C, 9} ,
  //{IO_PORT_B, 5} ,  // Test amaçlý(B5)
  //{IO_PORT_A, 3} ,
  
  // LCD
  {IO_PORT_B, 13},
  {IO_PORT_B, 14},
  {IO_PORT_A, 8},
  {IO_PORT_A, 9},
  {IO_PORT_A, 10},
  {IO_PORT_A, 11},
  
  // UART
  {IO_PORT_B, 7},
  {IO_PORT_B, 6},
  {IO_PORT_A, 3},
  {IO_PORT_A, 2},
};

IO_MODE _iom[] = {
  {IO_MODE_OUTPUT, IO_OTYPE_PP, IO_PUPD_NOPULL},
  //{IO_MODE_OUTPUT, IO_OTYPE_PP, IO_PUPD_NOPULL},
  //{IO_MODE_INPUT, IO_OTYPE_PP, IO_PUPD_NOPULL},
  
  {IO_MODE_OUTPUT, IO_OTYPE_PP, IO_PUPD_NOPULL},
  {IO_MODE_OUTPUT, IO_OTYPE_PP, IO_PUPD_NOPULL},
  {IO_MODE_OUTPUT, IO_OTYPE_PP, IO_PUPD_NOPULL},
  {IO_MODE_OUTPUT, IO_OTYPE_PP, IO_PUPD_NOPULL},
  {IO_MODE_OUTPUT, IO_OTYPE_PP, IO_PUPD_NOPULL},
  {IO_MODE_OUTPUT, IO_OTYPE_PP, IO_PUPD_NOPULL},
  
  {IO_MODE_AF, IO_OTYPE_PP, IO_PUPD_NOPULL},        // UART1 RX, hoca rx'i input yapýyor.
  {IO_MODE_AF, IO_OTYPE_PP, IO_PUPD_NOPULL},    // UART1 TX
  {IO_MODE_AF, IO_OTYPE_PP, IO_PUPD_NOPULL},
  {IO_MODE_AF, IO_OTYPE_PP, IO_PUPD_NOPULL},
};

GPIO_TypeDef *_GPIO_Ports[] = {
  GPIOA,
  GPIOB,
  GPIOC,
  GPIOD,
  GPIOE,
  GPIOF,
};

#endif

void IO_Init(int idx);
void IO_Write(int idx, int val);
int IO_Read(int idx); // burda dönüþ deðerinin int veya bool kullanýlmasýnýn farký yok.32 bitte olsa 1 bitte olsa 1 cycle'da iþliyor iþlemci.

#endif