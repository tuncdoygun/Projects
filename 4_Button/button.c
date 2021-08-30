#include <stdio.h>
#include <stdint.h>

#include "io.h"
#include "button.h"

int g_dbMax = 50; // parazit genisligi - 50ms
int _bScan = 0;   // tarama baslangic flag degeri

static BTN_PIN _bts[] = {
  {IOP_BTN_SET, 1, 0, 0}, // current statelerin 1 olmas�n�n sebebi pullup olmas�
  {IOP_BTN_UP, 1, 0, 0},
  {IOP_BTN_DN, 1, 0, 0},
};

#define N_BUTTONS (sizeof(_bts) / sizeof(*_bts))

unsigned g_Buttons[N_BUTTONS]; // semafor mekanizmas�nda kullan�lacak (binary / counting)
unsigned g_ButtonsL[N_BUTTONS]; // Long press semaphore

// Her "button timer" tick ile cagirilir.
// Tek bir butonun tarama islemini yapar.
static void BTN_Scan(int btIdx)
{
  int r; // pin okuma degeri
  
  r = IO_Read(_bts[btIdx].ioIdx);
  
  if(r != _bts[btIdx].cState){
    if(++_bts[btIdx].dbc >= g_dbMax){ // 50 kere ard arda buton durumu ayn� okunmal�.Bas�l� tutuldu�unda cState ile okunan de�er ayn� oldu�undan bu if'e girmez.
      // ba�ar� say�s�na ula��ld�: g�ncel durum de�i�tir.
      _bts[btIdx].cState = r;
      _bts[btIdx].dbc = 0;
      
      if (_bts[btIdx].cState == _bts[btIdx].aState){ // pullup ise aktif state = 0'd�r.
#ifdef BTN_LONG_PRESS
        _bts[btIdx].acc = 0;
#endif
        // Signal mekanizmas�, butona ba�ar�l� �ekilde bas�ld�.
        // g_Buttons[btIdx] = 1;   // binary semaphore
        ++g_Buttons[btIdx];     // counting semaphore
      }
    }
  }
  
#ifdef BTN_LONG_PRESS
  if(g_ButtonsL[btIdx] == 0) { // semaphore bayragi
    if(_bts[btIdx].cState == _bts[btIdx].aState) {
      if(++_bts[btIdx].acc >= BT_LP_TIME){
        _bts[btIdx].acc = 0;
        g_ButtonsL[btIdx] = 1; // binary semaphore        
      }
    }
  }
#endif
}

void BTN_InitButtons()
{
  int i;
  
  for(i = 0; i < N_BUTTONS; ++i)
    IO_Init(_bts[i].ioIdx);
  
  _bScan = 1;
}

void BTN_ScanButtons()
{
  if(!_bScan)
    return;
  
  int i;
  
  for(i = 0; i < N_BUTTONS; ++i)
    BTN_Scan(i);
}