#include <stdio.h>
#include <stdint.h>

#include "stm32f0xx.h"
#include "system.h"
#include "lcdm.h"
#include "uart.h"

#define __STDIN          0 // standart in file handles
#define __STDOUT         1
#define __STDERR         2

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

void DelayMs(unsigned int ms) // Sys_ClockInit fonksiyonunda _tmTick 1ms'de bir artar.
{
  clock_t t0 = clock();
  uint32_t delay_count = t0 + ms;

  while(_tmTick <= delay_count); 
}

void Sys_ClockInit(void)
{
  SysTick_Config(SystemCoreClock / CLOCKS_PER_SEC); // 1 ms kesme.
}

////////////////////////////////////////
// KONSOL FONKS�YONLARI
// printf'in nereye yazaca��n� belirtmek i�in alt seviyeli write callback fonksiyonunu bizim yazmam�z gerekiyor
// biz yazmasak da printf fonksiyonu var.Normalde write fonksiyonu var fakat i�i bo�,biz yaz�yoruz. __weak olarak tan�ml�
// �o�u derleyicide printf bu �ekilde �al���r.Yani __write fonksiyonunu �a��r�r.

void Sys_ConsoleInit()
{
  LCD_Init();
  UART_Init(g_conUART, 9600);
  //setvbuf(stdout, NULL, _IONBF, 0); // standart lib i�inde.Buffer'� devre d��� b�rakmak i�in.
									  // bazen buffer dolmad��� i�in ekranda bir�ey g�r�lm�yor.Bufferl� �al��t��� zaman h�zl� ama senkronlu�u kayboluyor.
}

void __putch(unsigned char c)
{
  LCD_putch(c);
}

size_t __write(int handle, const unsigned char *buffer, size_t size)
{
  size_t nChars = 0;
  
  if (buffer == NULL)
    return 0;
  
  if (handle != __STDOUT && handle != __STDERR)
    return 0;
  
  while(size--) {
    __putch(*buffer++);
    ++nChars;
  }
  
  return nChars;
}