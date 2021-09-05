#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
// #include "lcdm.h"
#include "uart.h"
#include "button.h"

void init(void)
{
  // System Clock init
  Sys_ClockInit();
  
  // I/O Init
  Sys_IoInit();
  
  // LED Init
  IO_Write(IOP_LED, 0);
  IO_Init(IOP_LED);
  
  // LCDM Ba�lang��
  // LCD_Init();
  
  // Konsol ba�lang��
  Sys_ConsoleInit();
  
  // Button baslangic
  BTN_InitButtons();
}

void Task_LED(void)
{
  static enum {
    I_LED_OFF,// led_off durumuna ge�erken ilk yap�lacaklar.ba�lang��ta yap�lacak i�ler �ok fazlaysa, nefes ald�rmak i�in daha faydal�.
    S_LED_OFF,
    I_LED_ON,
    S_LED_ON,
  } state = I_LED_OFF;
   
  static clock_t t0, t1; // t0 duruma ilk ge�i� saati, t1 g�ncel saat
  
  t1 = clock(); // bu fonksiyona girdi�inde o andaki saat
  
  switch (state){
  case I_LED_OFF:
    t0 = t1;
    IO_Write(IOP_LED, 0);
    state = S_LED_OFF;
    //break;
  case S_LED_OFF:
    if (t1 >= t0 + 9 * (CLOCKS_PER_SEC / 10)){ // 9/10 saniye ge�mi� demek. 900 ms i�in.��nk� 1 us'de bir kesme olu�uyor.
      state = I_LED_ON;
    }
    break;
    
  case I_LED_ON:
    t0 = t1;
    IO_Write(IOP_LED, 1);
    state = S_LED_ON;
    //break;
  case S_LED_ON:
    if (t1 >= t0 + 1 * (CLOCKS_PER_SEC / 10)){ // 9/10 saniye ge�mi� demek.100 ms i�in.��nk� 1 us'de bir kesme olu�uyor.
      state = I_LED_OFF;
    }    
    break;
  }
}
//

void Task_Print(void)
{
  static unsigned count;
  
  //printf("\nSAYI:%10u", ++count); // putch fonksiyonu LCD'ye yazd��� i�in bu fonksiyon lcd'ye yazar.
  UART_printf("Count: %10lu\r", count++);
}

void Task_Button()
{
  static unsigned count = 0;
  
  if(g_Buttons[BTN_SET]){// BTN_SET'ten isaret geldi
    // SET isle
    UART_printf("SET (%u)\n", ++count);
    
    // g_Buttons[BTN_SET] = 0; // binary semaphore
    --g_Buttons[BTN_SET]; // s�f�ra gelene kadar hep i�leyecek
  }
  if (g_Buttons[BTN_UP]){ // BTN_UP'ten i�aret geldi
    // SET i�le
    UART_printf("UP (%u)\n", ++count);
    
    //g_Buttons[BTN_UP] = 0;
    --g_Buttons[BTN_UP]; // s�f�ra gelene kadar hep i�leyecek
  }
  
  if (g_Buttons[BTN_DN]){ // BTN_UP'ten i�aret geldi
    // DN i�le
    UART_printf("DN (%u)\n", ++count);
    
    //g_Buttons[BTN_DN] = 0;
    --g_Buttons[BTN_DN]; // s�f�ra gelene kadar hep i�leyecek
  }
  
  //////////////////////////////////////
#ifdef BTN_LONG_PRESS
  if (g_ButtonsL[BTN_SET]){
    // SET i�le
    UART_printf("SET_LONG (%u)\n", ++count);
    
    g_ButtonsL[BTN_SET] = 0; //binary semaphore
    //--g_Buttons[BTN_SET]; // s�f�ra gelene kadar hep i�leyecek
  }
  
  if (g_ButtonsL[BTN_UP]){ // BTN_UP'ten i�aret geldi
    // SET i�le
    UART_printf("UP_LONG (%u)\n", ++count);
    
    g_ButtonsL[BTN_UP] = 0;
    // --g_Buttons[BTN_UP]; // s�f�ra gelene kadar hep i�leyecek
  }
  
  if (g_ButtonsL[BTN_DN]){ // BTN_UP'ten i�aret geldi
    // DN i�le
    UART_printf("DN_LONG (%u)\n", ++count);
    
    g_ButtonsL[BTN_DN] = 0;
    // --g_Buttons[BTN_DN]; // s�f�ra gelene kadar hep i�leyecek
  }
#endif
}

int main()
{
  // Ba�lang�� yap�land�rmalar�
  init();
  
  
  while(1) {
    Task_LED();
    //Task_Print();
    //Task_Button();
    
  }
  //return 0;
}
