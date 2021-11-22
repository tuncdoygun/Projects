#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "oled.h"
#include "uart.h"
#include "iuart.h"
#include "gnss.h"

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
  
  // Uart console init
  //UART_Init(g_conUART, 9600);
  
  // IUART INIT
  //IUART_Init();
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
  static unsigned long count = 0;
  int row, col;
  
  OLED_GetCursor(&row, &col);
  OLED_SetCursor(0, 0);
  printf("Count: %lu", ++count);
  OLED_SetCursor(row, col);
}

int main()
{
  //char str[256];
  //int len;
  
  // Baþlangýç yapýlandýrmalarý
  init();
  
  /*len = sprintf(str, "Bu deneme kesmeli amacli gonderme islemi\n\r");
  IUART_SendData(str, len);*/
  //UART_Send(g_conUART, 'A');
  
  OLED_SetCursor(1, 0);
    
  while(1) {
    Task_LED();
    Task_Print();
    //Task_IUART();
    Task_GNSS();
  }
  //return 0;
}
