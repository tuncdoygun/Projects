#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "oled.h"
#include "timer.h"

#define SYS_CLOCK_FREQ  48000000

void init(void)
{
  // System Clock init
  Sys_ClockInit();
  
  // I/O Init
  Sys_IoInit();
  
  // LED Init
  IO_Write(IOP_LED, 0);
  IO_Init(IOP_LED);
  
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
  
  OLED_SetFont(FNT_LARGE);
  OLED_SetCursor(0, 0);
  printf("SAYI:%10u\n", ++count);
  printf("T1Count:%8u\n", g_T1Count);
  printf("T2Count:%8u\n", g_T2Count);
}

int main()
{
  // int c;
  
  // Baþlangýç yapýlandýrmalarý
  init();
  
  Timer_Init(TIMER_1, SYS_CLOCK_FREQ / 1000, 1000, 1); // 1 sn kesme
  Timer_IntConfig(TIMER_1, 3);
  Timer_Start(TIMER_1, 1);
  
  Timer_Init(TIMER_2, SYS_CLOCK_FREQ / 2000, 1000, 1); // 500 ms kesme
  Timer_IntConfig(TIMER_2, 3);
  Timer_Start(TIMER_2, 1);
  
  while(1) {
    Task_LED();
    Task_Print();
  }
  //return 0;
}
