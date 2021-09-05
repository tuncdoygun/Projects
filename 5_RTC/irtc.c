#include <stdio.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "irtc.h"

#define IRTC_FLAG 0xE3B8 // uydurulmus deger.BKP_DR1'e daha önce yazdýðýmýz deðer,tekrar okuduðumuzda ayný deðeri okuyorsak BKP kapanmamýþ demektir.
#define RTC_PS 32768

volatile int g_Time = 0;
volatile int g_RtcAlarm = 0;
volatile int g_RtcChanged = 0;

void IRTC_Init(void)
{
  // PWR Birimleri saat isareti
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  // Backup registerlarina erisim,resetten sonra BKP registerlarina erisim yapilamiyor,bu komutla erisim olmasi gerek.
  PWR_BackupAccessCmd(ENABLE);
  
  if(RTC_ReadBackupRegister(RTC_BKP_DR0) != IRTC_FLAG) { // RTC ilk kez baslatiliyor.(BKP domain power on reset)
    // Backup domain resetlenmis.
    RCC_BackupResetCmd(ENABLE); // BKP_DeInit()
    RCC_BackupResetCmd(DISABLE);
    
    // LSE'yi calistiriyoruz.
    RCC_LSEConfig(RCC_LSE_ON);
    
    // LSE calisana kadar bekle
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
    
    // RTC Clock kaynagi = LSE olsun.
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RCC_RTCCLKCmd(ENABLE);
    
    RTC_WriteProtectionCmd(DISABLE); // f0'da senkron fonksiyonundan once kullanin yaziyor.
    // RTC registerlarina erisim icin senkronlamak gerek.
    RTC_WaitForSynchro();
    
    // ARASTÝRMAK GEREK.
  }
}