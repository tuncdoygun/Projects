#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "system.h"
#include "io.h"
#include "oled.h"
#include "hi2c.h"

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

void DrawSin(void)
{
  int x, y;
  float fx, pi = 3.14159265359;
  
  // y = A * sin(2 * pi * x);
  for (x = 0; x < 128; ++x) {
    fx = 32 + 30.0 * sin(2 * pi * (x / 50.0));
    y = 63 - (int)(fx + 0.5);
    OLED_SetPixel(x, y, OLED_SETPIXEL);
  }
}

void Task_Print(void)
{
  static unsigned count;
  
  printf("SAYI:%10u\r", ++count);
}

void OLED_Test(void)
{
  // OLED_Start(0);
  // OLED_Start(1); basasagi cevirmek icin
  
  /* OLED_SetPixel(0, 0, OLED_SETPIXEL);
  OLED_SetPixel(0, 5, OLED_SETPIXEL);
  OLED_SetPixel(127, 63, OLED_SETPIXEL);
  OLED_SetPixel(61, 19, OLED_SETPIXEL);

  OLED_SetPixel(61, 19, OLED_INVPIXEL);
  OLED_SetPixel(61, 19, OLED_INVPIXEL);
  
  OLED_SetPixel(0, 0, OLED_CLRPIXEL);
  DelayMs(2000);
  
  OLED_SetPage(2);
  OLED_SetSegment(26);
  
  OLED_Data(0x7E);
  OLED_Data(0x11);
  OLED_Data(0x11);
  OLED_Data(0x11);
  OLED_Data(0x7E); 
  DelayMs(2000);
  
  c = OLED_GetPixel(0, 0);
  c = OLED_GetPixel(0, 1);
  
  OLED_Scroll(1);
  OLED_Scroll(1);
  OLED_Scroll(1);
  DelayMs(2000); */
  
  DrawSin();
  
  /*
  for (c = 0; c < NPGS * NSEG; ++c)
    _DspRam[c] = 0xAA;
  
  OLED_UpdateDisplay();  
  DelayMs(2000);
  
  
  OLED_SetFont(FNT_LARGE);
  OLED_SetCursor(0, 0);
  OLED_PutChar('D');
  OLED_PutChar('e');
  OLED_PutChar('n');
  OLED_NewLine();
  OLED_PutChar('e');
  OLED_PutChar('m');
  OLED_PutChar('e');  
  DelayMs(2000);
  */
  

  /*OLED_SetFont(FNT_LARGE);
  printf("Hello, world!\n");
  OLED_SetFont(FNT_SMALL);
  OLED_SetCursor(2, 0);
  printf("Merhaba dünya!\n");
  
  printf("ABCÇDEFGÐHIÝJKLMNOÖPRSÞTUÜVYZ\n");
  printf("abcçdefgðhýijklmnoöprsþtuüvyz\n");
  printf("012345678901234567890123456\n");
  OLED_SetFont(FNT_LARGE);*/
}

void I2C_Test(void)
{
  HI2C_Init(I2C_1, 100000);
  
  if(HI2C_Start(I2C_1, 0xA0)) { // 1010 000x
    HI2C_Stop(I2C_1);
    
    printf("24LC128 memory bulundu!\n");
  } else {
    printf("HATA: 24LC128!\n");
  }
           
  /* if (HI2C_Start(I2C_1, 0x78)) {      // 1010 000x
    HI2C_Stop(I2C_1);
    
    printf("SSD1306 bulundu!\n");
  }
  else {
    printf("HATA: SSD1306!\n"); 
  } */ 
}

int main()
{
  // int c;
  
  // Baþlangýç yapýlandýrmalarý
  init();
  
  //OLED_Test();
  
  OLED_SetFont(FNT_SMALL);
  OLED_SetCursor(2, 0);
  I2C_Test();

  while(1) {
    Task_LED();
    Task_Print();
  }
  //return 0;
}
