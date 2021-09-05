#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "lcdm.h"
#include "uart.h"

void init(void)
{
  // System Clock init
  Sys_ClockInit();
  
  // I/O Init
  Sys_IoInit();
  
  // LED Init
  IO_Write(IOP_LED, 0);
  IO_Init(IOP_LED);
  
  // LCDM Baþlangýç
  // LCD_Init();
  
  // Konsol baþlangýç
  Sys_ConsoleInit();
}

//int c; // debug modda global deðiþkenlerin deðeri görülebiliyor.IO_Read() için.
// 27.07.2021
/*void Task_LED(void)
{
  static enum {
    S_LED_OFF,
    S_LED_ON,
  } state = S_LED_OFF;
   
  static clock_t t0, t1; // t0 duruma ilk geçiþ saati, t1 güncel saat
  
  t1 = clock(); // bu fonksiyona girdiðinde o andaki saat
  
  switch (state){
  case S_LED_OFF:
    if (t1 >= t0 + 9000 * CLOCKS_PER_SEC / 10){ // 9/10 saniye geçmiþ demek
      IO_Write(IOP_LED, 1);
      
      t0 = t1;
      state = S_LED_ON;
    }
    break;
  
  case S_LED_ON:
    if (t1 >= t0 +  1000 * CLOCKS_PER_SEC / 10){ // 9/10 saniye geçmiþ demek
      IO_Write(IOP_LED, 0);
      
      t0 = t1;
      state = S_LED_OFF;
    }    
    break;
  }
}*/
//

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
    if (t1 >= t0 + 9 * (CLOCKS_PER_SEC / 10)){ // 9/10 saniye geçmiþ demek. 900 ms için.Çünkü 1 us'de bir kesme oluþuyor.
      state = I_LED_ON;
    }
    break;
    
  case I_LED_ON:
    t0 = t1;
    IO_Write(IOP_LED, 1);
    state = S_LED_ON;
    //break;
  case S_LED_ON:
    if (t1 >= t0 + 1 * (CLOCKS_PER_SEC / 10)){ // 9/10 saniye geçmiþ demek.100 ms için.Çünkü 1 us'de bir kesme oluþuyor.
      state = I_LED_OFF;
    }    
    break;
  }
}
//

void Task_Print(void)
{
  static unsigned count;
  
  printf("\nSAYI:%10u", ++count); // putch fonksiyonu LCD'ye yazdýðý için bu fonksiyon lcd'ye yazar.
  UART_printf("Count: %10lu\r", count++);
}

int main()
{
  // Baþlangýç yapýlandýrmalarý
  init();
    
  /*UART_Send(g_conUART, 'A');
  DelayMs(1000);
  UART_Send(g_conUART, 'B');
  DelayMs(1000);
  UART_Send(g_conUART, 'C');
  DelayMs(1000);
  UART_Send(g_conUART, 'D');
  DelayMs(1000);*/
  
  UART_puts("\n\nMerhaba dunya!\n");
  
  while(1) {
    Task_LED();
    Task_Print();
       
    /*{ // Echo test
      unsigned char c;
        if(UART_DataReady(g_conUART)){
          c = UART_Recv(g_conUART);
          UART_Send(g_conUART, c);
        }
    }*/
    /*{
      unsigned char c;
      if(UART_DataReady(g_conUART)){
        UART_putch('\n');
        do {
          c = UART_Recv(g_conUART);
          UART_Send(g_conUART, c);
        } while(c != '\r');
        UART_putch('\n');
      }
    }*/
    
  }
  //return 0;
}
