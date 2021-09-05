#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "lcdm.h"

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
  
  printf("\nSAYI:%10u", ++count);
}

int main()
{
  // Baþlangýç yapýlandýrmalarý
  init();
    
  // printf("Hello World\n");
  
  // 03.08.2021
  LCD_DisplayOn(LCD_MODE_ON | LCD_MODE_CURSOR);
  
  LCD_SetCursor(0x00);
  LCD_PutChar('D'); // cursor otomatik 1 arttýðýndan dolayý
  LCD_PutChar('e');
  LCD_PutChar('n');
  LCD_PutChar('e');
  LCD_PutChar('m');
  LCD_PutChar('e');
  
  LCD_SetCursor(0x40); // 64: ikinci satýr baþý
  LCD_PutChar('T'); // cursor otomatik 1 arttýðýndan dolayý
  LCD_PutChar('e');
  LCD_PutChar('s');
  LCD_PutChar('t');
  
  
  while(1) {
    Task_LED();
    Task_Print();
  }
  //return 0;
}
