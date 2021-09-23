#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "spi.h"

#include "oled.h"

#define NPGS    8       // page
#define NSEG    128     // segment 

static int _page, _segment; // aktif page ve segmentin hangisi olacagini gosterecek.

static uint8_t _DspRam[NPGS * NSEG]; // ssd1306'daki RAM'in bir kopyasini biz tutucaz.
                                     // okuma yapmadigimizdan bu dizi ile yoneticez.

// SSD1306 OLED display'e komut gonderir.
void OLED_Command(uint8_t cmd)
{
  IO_Write(IOP_OLED_DC, 0); // komut
  
#ifdef IO_OLED_CS
  IO_Write(IOP_OLED_CS, 0); // CS = 0, aktif
#endif
  
  SPI_Data(cmd);
  
#ifdef IO_OLED_CS
  IO_Write(IOP_OLED_CS, 1); // CS = 1, pasif
#endif
}

// OLED display'e pixel data gonderir.
// Her data byte 8 pixele karsilik gelir.
void OLED_Data(uint8_t data)
{
  IO_Write(IOP_OLED_DC, 1);
  
#ifdef IO_OLED_CS
  IO_Write(IOP_OLED_CS, 0);
#endif
  
  SPI_Data(data);
  
  _DspRam[_page * NSEG + _segment] = data;
  
  if(++_segment >= NSEG)
    _segment = 0;
  
#ifdef IO_OLED_CS
  IO_Write(IOP_OLED_CS, 1);
#endif
}

void OLED_SetPage(uint8_t page)
{
  page &= 0x07; // 7'den buyukse buyuk olan kisimlari kirpiliyor.
  
  _page = page;
  OLED_Command(0xB0 | page); // page set etme komutu
}

void OLED_SetSegment(uint8_t segment)
{
  segment &= 0x7F; // 128'den buyuk olmasini engellemek icin.
  
  _segment = segment;
  
  OLED_Command(segment & 0x0F); // once dusuk anlamlý bit gönderilir,spi bu sekilde çalisiyor
  OLED_Command(0x10 | (segment >> 4)); // segment set etme komutu.
}

void OLED_FillPage(uint8_t page, uint8_t ch)
{
  int i;
  
  OLED_SetPage(page);
  OLED_SetSegment(0);
  
  for(i = 0; i < NSEG; ++i)
    OLED_Data(ch);
}

void OLED_FillDisplay(uint8_t ch)
{
  int page;
  
  for(page = 0; page < NPGS; ++page)
    OLED_FillPage(page, ch);
}

void OLED_ClearDisplay(void)
{
  OLED_FillDisplay(0);
}

void OLED_Start(int bRotate)
{
  SPI_Start();
  
#ifdef IO_OLED_GND
  IO_Write(IOP_OLED_GND, 0);
  IO_Init(IOP_OLED_GND);
#endif
  
#ifdef IO_OLED_VDD
  IO_Write(IOP_OLED_CS, 1);
  IO_Init(IOP_OLED_VDD);
#endif
  
#ifdef IO_OLED_CS
  IO_Write(IOP_OLED_CS, 1);
  IO_Init(IOP_OLED_CS);
#endif
  
  IO_Init(IOP_OLED_DC);
  
#ifdef IO_OLED_RES
  // SSD1306 reset pulse
  IO_Write(IOP_OLED_RES, 0);
  IO_Init(IOP_OLED_RES);
  DelayMs(10);
  IO_Write(IOP_OLED_RES, 1);
#endif
  
  DelayMs(80);
  
  OLED_Command(0xAE); // Display Off
  
  OLED_Command(0xD4); // Clock ratio
  OLED_Command(0x80); // Freq
  
  OLED_Command(0xD3);
  OLED_Command(0x00);
  
  OLED_Command(0x40);
  
  OLED_Command(0x8D);
  OLED_Command(0x14);

  if(bRotate) {
    OLED_Command(0xA0);
    OLED_Command(0xC0);
  } else {
    OLED_Command(0xA1);
    OLED_Command(0xC8);
  }
  
  OLED_Command(0xDA);
  OLED_Command(0x12);

  OLED_Command(0xDA);
  OLED_Command(0x12);
  
  OLED_Command(0x81);
  OLED_Command(0xCF);  
  
  OLED_Command(0xD9);
  OLED_Command(0xF1);
  
  OLED_Command(0xDB);
  OLED_Command(0x40);

  OLED_Command(0xA4);
  OLED_Command(0xA6);
  OLED_Command(0xAF); 
  
  OLED_ClearDisplay();
}

void OLED_SetPixel(int x, int y, int c)
{
  uint8_t page, bitIdx, val;
  
  x &= 0x7F; // x en fazla 127
  y &= 0x3F; // y en fazla 63
  
  page = y >> 3; // kacinci page'de oldugunu bulmak icin.8'e bolumunun sonucu. page = y / 8;
  bitIdx = y & 7; // 8'e bolumunden kalan. bitIdx = y % 8;
  
  val = _DspRam[page * NSEG + x]; // soz konusu pixelin hangi ram bolgesine karsilik geldigi

  switch (c) {
  case OLED_SETPIXEL:
    val |= (1 << bitIdx);
    break;
    
  case OLED_CLRPIXEL:
    val &= ~(1 << bitIdx);
    break;
    
  case OLED_INVPIXEL:
    val ^= (1 << bitIdx);
    break;
  }
  
  OLED_SetPage(page);
  OLED_SetSegment(x);
  OLED_Data(val);
}