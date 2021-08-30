#ifndef _BUTTON_H
#define _BUTTON_H

#define BT_LP_TIME 3000
#define BTN_LONG_PRESS

typedef struct {
  int ioIdx;    // buttonun bagli olduðu input pin index
  int cState;   // current state, butonun guncel durumu, belirli periyotlarla okuma yapilacak,o okumanin degeri
  int aState;   // active state, aktif durum, aktifken sýfýr mý aktifken 1 mi
  int dbc;      // debounce counter, kac kere saydýktan sonra buton durumunu belirleyecegi
#ifdef BTN_LONG_PRESS
  int acc;      // active counter
#endif
} BTN_PIN;

enum {
  BTN_SET,
  BTN_UP,
  BTN_DN,
};

extern unsigned g_Buttons[];
extern unsigned g_ButtonsL[]; // long

void BTN_InitButtons(void);
void BTN_ScanButtons(void); // tek bir buton taramasini yapacak.

#endif