#include <stdio.h>
#include <stdint.h>

#include "stm32f0xx.h"
#include "system.h"

static volatile clock_t _tmTick;

static const int _GPIO_Clocks[] = {
  RCC_AHBPeriph_GPIOA,
  RCC_AHBPeriph_GPIOB,
  RCC_AHBPeriph_GPIOC,
  RCC_AHBPeriph_GPIOD,
  RCC_AHBPeriph_GPIOE,
  RCC_AHBPeriph_GPIOF,
};

#define N_PORTS (sizeof(_GPIO_Clocks) / sizeof(*_GPIO_Clocks))

void Sys_IoInit(void)
{
  int i;
  
  for(i = 0; i < N_PORTS; ++i) // B�t�n GPIO Clock i�aretlerinin a��lmas�.Extreme g�� tasarrufu gerekmedi�inden hepsini enable etmek bir�ey de�i�tirmez
    RCC_AHBPeriphClockCmd(_GPIO_Clocks[i], ENABLE);
  
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO Clock
  //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // JTAG disable.Biz STLINK kulland���m�zdan fazla pin kullanan JTAG'i disable ettik.
}

void Sys_ClockTick(void)
{
  // elapsed time counter.1 ms'de bir de�er 1 artacak.
  ++_tmTick;
}

clock_t clock(void)
{
  return _tmTick;
}

void Sys_ClockInit(void)
{
  SysTick_Config(SystemCoreClock / CLOCKS_PER_SEC); // 1 ms kesme.
}