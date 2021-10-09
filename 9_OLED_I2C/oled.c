#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "spi.h"

#include "oled.h"
#include "font_5x8.h"
#include "font_7x16.h"

#define T_COLS  (NSEG / ((_szWidth) + 1)) // displayda kac tane text sutun olacak
#define T_ROWS  (NPGS / (_szHeight))     // displayda kac tane text satir olacak

static int _szWidth;    // aktif font pixel genisligi (bit)
static int _szHeight;   // aktif font yuksekligi (page - byte)
static const unsigned char *_pFont; // aktif font tablosu
static int _row, _col;  // text satir, sutun
static int _font;
static int _page, _segment; // aktif page ve segmentin hangisi olacagini gosterecek.

uint8_t _DspRam[NPGS * NSEG]; // ssd1306'daki RAM'in bir kopyasini biz tutucaz.
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

void OLED_Rotate(int bRotate)
{
  unsigned char remap, scan;
  
  if(bRotate) {
    remap = 0xA0;
    scan = 0xC0;
  } else {
    remap = 0xA1;
    scan = 0xC8;
  }
  
  OLED_Command(remap); // Set Segment Re-Map
  OLED_Command(scan);  // Set Com Output Scan Direction
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
  
  OLED_Command(0xAE); // Set display OFF
  
  OLED_Command(0xD4); // Set Display Clock Divide Ratio / OSC Frequency
  OLED_Command(0x80); // Display Clock Divide Ratio / OSC Frequency 
  
  OLED_Command(0xA8); // Set Multiplex Ratio
  OLED_Command(0x3F); // Multiplex Ratio for 128x64 (64-1)

  OLED_Command(0xD3); // Set Display Offset
  OLED_Command(0x00); // Display Offset

  OLED_Command(0x40); // Set Display Start Line

  OLED_Command(0x8D); // Set Charge Pump
  OLED_Command(0x14); // Charge Pump (0x10 External, 0x14 Internal DC/DC)

  OLED_Rotate(bRotate);

  OLED_Command(0xDA); // Set COM Hardware Configuration
  OLED_Command(0x12); // COM Hardware Configuration

  OLED_Command(0x81); // Set Contrast
  OLED_Command(0x80); // Contrast

  OLED_Command(0xD9); // Set Pre-Charge Period
  OLED_Command(0xF1); // Set Pre-Charge Period (0x22 External, 0xF1 Internal)

  OLED_Command(0xDB); // Set VCOMH Deselect Level
  OLED_Command(0x40); // VCOMH Deselect Level

  OLED_Command(0xA4); // Enable display outputs according to the GDDRAM contents
  OLED_Command(0xA6); // Set display not inverted
  
  OLED_ClearDisplay();

  OLED_Command(0xAF); // Set display On
  
  //////////////////////////////////////
  // Set default font
  OLED_SetFont(FNT_SMALL);
}

// Bizdeki RAM displaydaki RAM'den farklýysa guncelle
void OLED_UpdateDisplay(void)
{
  int i, page, segment;
  
  for (i = page = 0; page < NPGS; ++page) {
    OLED_SetPage(page);
    OLED_SetSegment(0);
    
    for(segment = 0; segment < NSEG; ++segment)
      OLED_Data(_DspRam[i++]); 
  }
}

void OLED_Scroll(int nLines)
{
  int i; // kopya hedefi
  int j; // kopya kaynagi
  
  j = nLines * NSEG;
  
  for(i = 0; i < (NPGS - nLines) * NSEG; ++i)
    _DspRam[i] = _DspRam[j++];
  
  for( ; i < NPGS * NSEG; ++i)
    _DspRam[i] = 0;
  
  OLED_UpdateDisplay();
}

void OLED_SetFont(int font)
{
  switch (font) {
  case FNT_SMALL:
    _szWidth = 5;
    _szHeight = 1;
    _pFont = g_ChrTab;
    _font = FNT_SMALL;
    break;
    
  case FNT_LARGE:
    _szWidth = 7;
    _szHeight = 2;
    _pFont = g_ChrTab2;
    _font = FNT_LARGE;
    break;
  }
}

int OLED_GetFont(void)
{
  return _font;
}

void OLED_SetCursor(int row, int col)
{
  _row = row;
  _col = col;
}

// Resim cizmek icin
void OLED_DrawBitmap(const uint8_t *pBitmap)
{
  int i;
  
  for(i = 0; i < NPGS * NSEG; ++i)
    _DspRam[i] = pBitmap[i];
  
  OLED_UpdateDisplay();
}

// CR islevini yapmak icin
void OLED_Return(void)
{
  _col = 0;
}

// \n islevi
void OLED_NewLine(void)
{
  if (++_row >= T_ROWS) {
    _row = T_ROWS - 1;
    
    OLED_Scroll(_szHeight);
  }
}

void OLED_PutChar(char c)
{
  int i, j, k;
    
  if (_col >= T_COLS) {
    OLED_Return();
    OLED_NewLine();
  }
  
  for (k = 0; k < _szHeight; ++k) {
    OLED_SetPage(_row * _szHeight + k);
    OLED_SetSegment(_col * (_szWidth + 1));
    
    i = _szWidth * _szHeight * c + k;
    
    for (j = 0; j < _szWidth; ++j) {
      OLED_Data(_pFont[i]);
      i += _szHeight;
    }
                
    OLED_Data(0);
  }
                
  ++_col;
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

// pixel ON mu OFF mu
int OLED_GetPixel(int x, int y)
{
  uint8_t page, bitIdx, val;
  
  x &= 0x7F; // x en fazla 127
  y &= 0x3F; // y en fazla 63
  
  page = y >> 3; // kacinci page'de oldugunu bulmak icin.8'e bolumunun sonucu. page = y / 8;
  bitIdx = y & 7; // 8'e bolumunden kalan. bitIdx = y % 8. O segmentteki byte'in hangi bitine karsilik geldigi
  
  val = _DspRam[(unsigned)page * NSEG + x];
  
  return (val & (1 << bitIdx)) != 0;
}

/////////////////////////////////

void OLED_putch(char c)
{
  switch (c) {
  case '\n':
    OLED_NewLine();
    
  case '\r':
    OLED_Return();
    break;
    
  case '\f':
    OLED_ClearDisplay();
    break;
    
  default:
    OLED_PutChar(c);
    break;
  }
}
