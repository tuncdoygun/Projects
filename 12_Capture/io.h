#ifndef _IO_H
#define _IO_H

#include "stm32f0xx_gpio.h"

#define IO_OLED_RES // kullanýlacak chipte cs veya res olup olmama durumu icin
#define IO_OLED_CS

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
  
  // SPI1
  IOP_SPI_SCK,
  IOP_SPI_MISO,
  IOP_SPI_MOSI,
  
  // OLED
#ifdef IO_OLED_VDD
  IOP_OLED_VDD, // yazilimsal spi kullanilacaksa, yani VDD GPIO'ya baglandiysa
#endif
  
#ifdef IO_OLED_GND
  IOP_OLED_GND, // yazýlýmsal spi kullanýlacaksa,yani GND GPIO'ya baðlandýysa
#endif
  
#ifdef IO_OLED_RES
  IOP_OLED_RES,
#endif
  
  IOP_OLED_DC,
  
#ifdef IO_OLED_CS
  IOP_OLED_CS,
#endif
  
  IOP_TIM4_CH1,
};

#ifdef _IOS_ // birkere tanýmlanmasý gereken

IO_PIN _ios[] = {
  {IO_PORT_C, 9} ,
  
  // SPI1
  {IO_PORT_A, 5}, // SCK
  {IO_PORT_A, 6}, // MISO
  {IO_PORT_A, 7}, // MOSI
  
  // OLED
#ifdef IO_OLED_VDD
#endif
  
#ifdef IO_OLED_GND
#endif
  
#ifdef IO_OLED_RES
  {IO_PORT_C, 0}, // RES
#endif
  
  {IO_PORT_C, 1}, // DC
  
#ifdef IO_OLED_CS
  {IO_PORT_C, 2}, // CS
#endif
  
  {IO_PORT_C, 9}, // TIM14_CH1
};

IO_MODE _iom[] = {
  {IO_MODE_OUTPUT, IO_OTYPE_PP, IO_PUPD_NOPULL}, // Bluepill LED (C13)
  {IO_MODE_AF, IO_OTYPE_PP, IO_PUPD_DOWN},       // SCK
  {IO_MODE_AF, IO_OTYPE_PP, IO_PUPD_DOWN},       // MOSI
  {IO_MODE_AF, IO_OTYPE_PP, IO_PUPD_DOWN},       // MISO -- Hoca input olarak tanimlamis.Calismadi.
  {IO_MODE_OUTPUT, IO_OTYPE_PP, IO_PUPD_NOPULL}, // RES
  {IO_MODE_OUTPUT, IO_OTYPE_PP, IO_PUPD_NOPULL}, // DC
  {IO_MODE_OUTPUT, IO_OTYPE_PP, IO_PUPD_NOPULL}, // CS 
  {IO_MODE_INPUT, IO_OTYPE_PP, IO_PUPD_NOPULL},  // TIM14_CH1(capture)
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