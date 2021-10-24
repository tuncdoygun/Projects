#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "hi2c.h"

#define HI2C_TIMEOUT    1000 

static I2C_TypeDef      *_i2c[] = {
  I2C1,
  I2C2,
};

enum { FALSE, TRUE};

// Hardware I2C baslatma
void HI2C_Init(int nI2C, int bRate)
{
  I2C_InitTypeDef i2cInit;
  I2C_TypeDef *pI2C = _i2c[nI2C];
  
  if(nI2C == I2C_1) {
    // 2) SCL ve SDA I/O uclari yapilandirilacak. bazi durumlarda ilk clock baslatinca hata olabiliyor.
    // o sebepten i2c'de ilk io uclarini baslatip sonra clock baslatiyoruz.
    IO_Init(IOP_I2C1_SCL);
    IO_Init(IOP_I2C1_SDA);
    
    // 1) I2C cevresel birim clock aktif olmali
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  }
  else if(nI2C == I2C_2) {
    // 2) SCL ve SDA I/O uclari yapilandirilacak. bazi durumlarda ilk clock baslatinca hata olabiliyor.
    // o sebepten i2c'de ilk io uclarini baslatip sonra clock baslatiyoruz.
    IO_Init(IOP_I2C2_SCL);
    IO_Init(IOP_I2C2_SDA);
    
    // 1) I2C cevresel birim clock aktif olmali
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
  }
  
  // 3) I2C baslangic parametreleri yapilandirilacak
  i2cInit.I2C_Ack = I2C_Ack_Enable;
  i2cInit.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  i2cInit.I2C_AnalogFilter = I2C_AnalogFilter_Disable;
  i2cInit.I2C_DigitalFilter = 0;
  i2cInit.I2C_Mode = I2C_Mode_I2C;
  i2cInit.I2C_OwnAddress1 = 0x55;
  I2C_Init(pI2C, &i2cInit);
  
  // 4) Cevresel birim aktif (on)
  I2C_Cmd(pI2C, ENABLE); 
}

// bir islem yapiliyor,yapildi mi diye Event flag'i kontrol ediliyor.
// stm32f0'da boyle bir arayuz yok
/*
static int HI2C_Event(int nI2C, int event)
{
  unsigned long tmOut = HI2C_TIMEOUT;
  I2C_TypeDef *pI2C = _i2c[nI2C];
  
  do {
    if(I2C_CheckEvent(pI2C, event) == SUCCESS)
      break;
  } while(--tmOut);
}
*/

void HI2C_Stop(int nI2C)
{
  I2C_TypeDef *pI2C = _i2c[nI2C];
  
  //Wait until I2C isn't busy
  while(I2C_GetFlagStatus(pI2C, I2C_FLAG_BUSY) == SET);
  
  I2C_GenerateSTOP(pI2C, ENABLE);
}

// Start kosulu olusturur, ardindan kontrol byte'i gönderir
int HI2C_Start(int nI2C, unsigned char ctl)
{
  I2C_TypeDef *pI2C = _i2c[nI2C];
  
  //Wait until I2C isn't busy
  while(I2C_GetFlagStatus(pI2C, I2C_FLAG_BUSY) == SET);
 
  I2C_GenerateSTART(pI2C, ENABLE);
  //I2C_TransferHandling(pI2C, 0, 1, I2C_Reload_Mode, I2C_Generate_Start_Write);

  //Ensure the transmit interrupted flag is set
  while(I2C_GetFlagStatus(pI2C, I2C_FLAG_TXIS) == RESET); // Burada kaliyor.Daha sonra incele
  
  I2C_SendData(pI2C, ctl);
  
  //Ensure that the transfer complete reload flag is
  //set, essentially a standard TC flag
  //while(I2C_GetFlagStatus(pI2C, I2C_FLAG_TCR) == RESET);  
  if (I2C_GetFlagStatus(pI2C, I2C_FLAG_TCR))
    return 1;
  
  return 0;
}
  
// I2C bus uzerinden 8-bit veri gönderir
void HI2C_Send(int nI2C, unsigned char val)
{
  I2C_TypeDef *pI2C = _i2c[nI2C];
  
  //Wait until I2C isn't busy
  while(I2C_GetFlagStatus(pI2C, I2C_FLAG_BUSY) == SET);

  I2C_TransferHandling(pI2C, 0, 1, I2C_Reload_Mode, I2C_No_StartStop);
  
  I2C_SendData(pI2C, val);
  
  //Ensure that the transfer complete reload flag is
  //set, essentially a standard TC flag
  while(I2C_GetFlagStatus(pI2C, I2C_FLAG_TCR) == RESET);    
}
  
int HI2C_Recv(int nI2C, unsigned char *pVal)
{
  I2C_TypeDef *pI2C = _i2c[nI2C];
  
  while(I2C_GetFlagStatus(pI2C, I2C_FLAG_BUSY) == SET);
  
  I2C_TransferHandling(pI2C, 0, 1, I2C_Reload_Mode, I2C_Generate_Start_Read);
  
  while(I2C_GetFlagStatus(pI2C, I2C_FLAG_RXNE) == RESET);
  
  *pVal = I2C_ReceiveData(pI2C);
  
  return TRUE;
}
    
// Slave'e ack gönderme veya yapilandirmasi
// ack: ACK biti lojik deðeri 0: olumlu
void HI2C_Acknowledge(int nI2C, int ack)
{
  I2C_TypeDef *pI2C = _i2c[nI2C];

  I2C_AcknowledgeConfig(pI2C, ack ? DISABLE : ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
// GENERIC I2C FUNCTIONS

// I2C Generic Write
// I2C uzerinden yazma cumlesi ile veri gönderir
// nI2C: I2C cevresel no
// devAdr: Slave device hardware 7-bit address (high 7-bit)
// buf: Gonderilecek verinin baslangic adresi
// len: Gonderilecek veri uzunlugu (kontrol byte haric)
int HI2C_Write(int nI2C, unsigned char devAdr, const void *buf, int len)
{
  // CALISTIGINI GORDUGUNDE ICERDEKI KONTROLLERI YAP.HOCANIN HI2C.C DOSYASINDAN BAKARAK.
  const unsigned char *ptr = (const unsigned char *)buf;
  
  devAdr &= 0xFE;
  
  HI2C_Start(nI2C, devAdr);
  
  while(len--)
    HI2C_Send(nI2C, *ptr++);
  
  HI2C_Stop(nI2C);

  return TRUE; // return (len == 0)
}

int HI2C_Read()
{
  return TRUE;
}