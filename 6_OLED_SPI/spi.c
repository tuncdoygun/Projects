#include <stdio.h>
#include <stdint.h>

#include "stm32f0xx_spi.h"
#include "system.h"
#include "io.h"

// SOFTWARE SPI

///////////////////////////////////////////////////

#define SPI_PORT        SPI1

// HARDWARE SPI

// Donan�msal SPI cevresel birimini baslatir.
void HSPI_Start(void) // bastaki H hardware
{
  SPI_InitTypeDef spiInit;
  
  // 1 - SPI Clock aktif edilir.
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  
  // 2 - SPI GPIO pinleri yap�land�r�l�r.
  IO_Init(IOP_SPI_SCK);
  IO_Init(IOP_SPI_MOSI);
  IO_Init(IOP_SPI_MISO);
  
  // 3 - SPI Init structure parametreleri yapilandirilir.
  spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  spiInit.SPI_CPHA = SPI_CPHA_1Edge; // Mode-0
  spiInit.SPI_CPOL = SPI_CPOL_Low;
  spiInit.SPI_DataSize = SPI_DataSize_8b;
  spiInit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  spiInit.SPI_FirstBit = SPI_FirstBit_MSB;
  spiInit.SPI_Mode = SPI_Mode_Master;
  spiInit.SPI_NSS = SPI_NSS_Soft; // hardware olarak da kullan�labiliyor.
  
  SPI_Init(SPI_PORT, &spiInit);
  
  // 4 - SPI cevresel birimi baslatilir.
  SPI_Cmd(SPI_PORT, ENABLE);
}

// SPI �zerinden 8 bit veri g�nderir ve al�r.
// sadece veri almak icin dummy bir veri gonderilebilir.
uint8_t HSPI_Data(uint8_t val)
{
  // SPI Transmit buffer bos mu ? TXE Flag 1 olana kadar bekle.
  while(!SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE));
  
  SPI_SendData8(SPI_PORT, val);
  
  //while(!SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_RXNE)); // normalde almadan ��kmaz gibi dursa da,tx ile rx ayn� anda �al���yor.

  //val = SPI_ReceiveData8(SPI_PORT);
  
  return val;
}