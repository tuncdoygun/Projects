#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "oled.h"

void init(void)
{
  // System Clock init
  Sys_ClockInit();
  
  // I/O Init
  Sys_IoInit();
  
  // LED Init
  IO_Write(IOP_LED, 0);
  IO_Init(IOP_LED);
  
  // Button Init
  IO_Init(IOP_BTN1);
  
  // Konsol Init
  Sys_ConsoleInit();
}

// 29.07.2021
// yukardakine göre daha optimize
void Task_LED(void)
{
  static enum {
    I_LED_OFF,// led_off durumuna geçerken ilk yapýlacaklar.baþlangýçta yapýlacak iþler çok fazlaysa, nefes aldýrmak için daha faydalý.
    S_LED_OFF,
    I_LED_ON,
    S_LED_ON,
  } state = I_LED_OFF;
   
  static clock_t t0, t1; // t0 duruma ilk geçiþ saati, t1 güncel saat
  
  t1 = clock(); // bu fonksiyona girdiðinde o andaki saat
  
  switch (state){
  case I_LED_OFF:
    t0 = t1;
    IO_Write(IOP_LED, 0);
    state = S_LED_OFF;
    //break;
  case S_LED_OFF:
    if (t1 >= t0 + 9 * CLOCKS_PER_SEC / 10){ // 9/10 saniye geçmiþ demek
      state = I_LED_ON;
    }
    break;
    
  case I_LED_ON:
    t0 = t1;
    IO_Write(IOP_LED, 1);
    state = S_LED_ON;
    //break;
  case S_LED_ON:
    if (t1 >= t0 +  CLOCKS_PER_SEC / 10){ // 9/10 saniye geçmiþ demek
      state = I_LED_OFF;
    }    
    break;
  }
}
//

void Task_Print(void)
{
  static unsigned count;
  
  OLED_SetCursor(0, 0);
  printf("SAYI:%10u\r", ++count);
}

void ResetProc(void)
{
  char *msg;
  
  if (RCC_GetFlagStatus(RCC_FLAG_PORRST)) 
    msg = "Power On";
  else if (RCC_GetFlagStatus(RCC_FLAG_SFTRST))
    msg = "Software";
  else if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST))
    msg = "Indp WDG";
  else if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST))
    msg = "Wind WDG";
  else if (RCC_GetFlagStatus(RCC_FLAG_LPWRRST))
    msg = "LowPower";
  else if (RCC_GetFlagStatus(RCC_FLAG_PINRST)) // external sonda olmasi gerek.
    msg = "External";
  
  OLED_SetCursor(1, 0);
  printf(msg);      
  
  RCC_ClearFlag();      // !!!
}

// tmr : reset araliginin ne kadar oldugu
void IWDG_Init(uint16_t tmr)
{
  // 1) IWDG register'larýna yazma izni olmalý
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  
  // 2) IWDG clock prescale ayarý (LSI)
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  
  // 3) Reload (periyot) deðeri
  IWDG_SetReload(tmr);  // 12-bit
  
  // 4) Sayacý yüklüyoruz
  IWDG_ReloadCounter();
  
  // 5) IWDG birimini aktif hale getiriyoruz
  IWDG_Enable();
}

int main()
{
  // Baþlangýç yapýlandýrmalarý
  init();

  ResetProc();
  
  IWDG_Init(0xFFF); // en yuksek degeri, 25 sn civari
  while (1)
  {
    Task_LED();
    Task_Print();
    
    if (g_Buttons[0]) {
      IWDG_ReloadCounter();
      g_Buttons[0] = 0;
    }

    /*if (g_Buttons[0]) 
      NVIC_SystemReset(); */       // Software Reset  
    
    //IWDG_ReloadCounter();
  }
  //return 0;
}
