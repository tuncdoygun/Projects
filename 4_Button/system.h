#define CLOCKS_PER_SEC 1000

typedef uint32_t clock_t;

clock_t clock(void);

void Sys_ClockInit(void);
void Sys_IoInit(void);
void DelayMs(unsigned int); // td
void Sys_ConsoleInit(void);

// 29.07.2021 - hocanýn delay.s fonksiyonu için.
/*
void __delay(uint32_t ncy); // number of cycles

#define CLOCK_FREQ      48000000
#define INSTR_FREQ      ((CLOCK_FREQ) * 8 / 9) //instructor frequency, bir komutun iþleme süresi.
#define US_CYCLES       ((INSTR_FREQ) / 1000000)
#define MS_CYCLES       ((INSTR_FREQ) / 1000)

#define DelayUs(us)     __delay((us) * US_CYCLES) // timer ile gecikme yapýldýðýnda kesme,ayarlamalar vs zaman alabilir kritik zamanlarda.
#define DelayMs(ms)     __delay((ms) * MS_CYCLES)
*/
//