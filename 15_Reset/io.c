#include <stdio.h>
#include <stdint.h>

#include "stm32f0xx.h"

#define _IOS_
#include "io.h"

static int g_dbMax = 5;

void IO_Init(int idx)
{
  GPIO_InitTypeDef ioInit;
  int port;
  
  ioInit.GPIO_Mode = (GPIOMode_TypeDef)_iom[idx].mode;
  ioInit.GPIO_Speed = GPIO_Speed_Level_3;
  ioInit.GPIO_OType = (GPIOOType_TypeDef)_iom[idx].otype;
  ioInit.GPIO_PuPd = (GPIOPuPd_TypeDef)_iom[idx].pupd;
  ioInit.GPIO_Pin = (1 << _ios[idx].pin);
  
  port = _ios[idx].port;
  GPIO_Init(_GPIO_Ports[port], &ioInit);
}

void IO_Write(int idx, int val)
{
  int port;
  port = _ios[idx].port;
  
  if(val)
    _GPIO_Ports[port]->BSRR = (1 << _ios[idx].pin);
  else
    _GPIO_Ports[port]->BRR = (1 << _ios[idx].pin);
}

int IO_Read(int idx)
{
  int port;
  port = _ios[idx].port;
  
  return (_GPIO_Ports[port]->IDR & (1 << _ios[idx].pin)) != 0;
}

///////////////////////////////////////////////
// PUSH BUTTON FONKSÝYONLARI

void IO_Scan(int btIdx)
{
  int r;
  
  r = IO_Read(_bts[btIdx].idx);
  
  if (r != _bts[btIdx].cState) {
    if (++_bts[btIdx].dbc >= g_dbMax) {
      _bts[btIdx].cState = r;
      _bts[btIdx].dbc = 0;
      
      if (_bts[btIdx].cState == _bts[btIdx].aState) {
        // Sinyal
        g_Buttons[btIdx] = 1; // Binary semaphore
        //++g_Buttons[btIdx];     // Counting semaphore
      }
    }
  }
  else 
    _bts[btIdx].dbc = 0;
}

void IO_ScanButtons(void)
{
  int i;
  
  for (i = 0; i < N_BUTTONS; ++i)
    IO_Scan(i);
}
