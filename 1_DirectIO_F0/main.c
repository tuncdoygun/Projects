#include <stdint.h>

#define PERIPH_BASE             ((uint32_t)0x40000000)
#define APB1PERIPH_BASE         PERIPH_BASE
#define APB2PERIPH_BASE         (PERIPH_BASE + 0x10000)
#define AHB1PERIPH_BASE         (PERIPH_BASE + 0x20000)
#define AHB2PERIPH_BASE         (PERIPH_BASE + 0x8000000)

#define RCC_BASE                (AHB1PERIPH_BASE + 0x1000)
#define GPIOC_BASE              (AHB2PERIPH_BASE + 0x800)

#define IOP_LED_PORT   GPIOC
#define IOP_LED_PIN    9

#define __IO            volatile

typedef struct {
/* 0 */  __IO uint32_t CR;
/* 4 */  __IO uint32_t CFGR;
/* 8 */  __IO uint32_t CIR;
  __IO uint32_t APB2RSTR;
  __IO uint32_t APB1RSTR;
  __IO uint32_t AHBENR;
  __IO uint32_t APB2ENR;
  __IO uint32_t APB1ENR;
  __IO uint32_t BDCR;
  __IO uint32_t CSR;
} RCC_TypeDef;

typedef struct {
  __IO uint32_t MODER;
  __IO uint32_t OTYPER;
  __IO uint32_t OSPEEDR;
  __IO uint32_t PUPDR;
  __IO uint32_t IDR;
  __IO uint32_t ODR;
  __IO uint32_t BSRR;
  __IO uint32_t LCKR;
  __IO uint32_t AFRL;
  __IO uint32_t AFRH;
  __IO uint32_t BRR;
} GPIO_TypeDef;

#define RCC_AHBENR_IOPCEN      ((uint32_t)0x00080000) /*!< I/O port C clock enable */

#define GPIOC                   ((GPIO_TypeDef *)GPIOC_BASE)
#define RCC                     ((RCC_TypeDef *)RCC_BASE)

void Wait(void)
{
}

void Delay(void)
{
  int i;
  
  for (i = 0; i < 1000000; ++i)
    Wait(); // derleyicinin burayý es geçmemesi için.
}

// Amaç: PC9'a baðlý LED'ý yakýp söndürmek. Bunun için PC9 Çýkýþ-0 Çýkýþ-1 yapýlmalý
// Not: Bu uygulama debug ortamýnda adým adým çalýþtýrýlmalýdýr. Program koþturulduðunda
// yanma/sönme tepki ve algý süreleri çok kýsa olacaðýndan durum anlaþýlmayacaktýr.

int main()
{
  // int bitOffset = IOP_LED_PIN;
  
  // 1) Saat iþaretini aç
  // Reset ve Clock Control register'ý clock saðlamak için kullanýlacak
  RCC->AHBENR |= RCC_AHBENR_IOPCEN;
  
  // 2) Portu yapýlandýr
  GPIOC->BSRR = (1 << IOP_LED_PIN); // DR<9> = 1
  
  // MODER[19,18] = MODER9 : 01
  // OTYPER<9> = 0
  // OSPEEDR[19, 18] = 11
  // PUPDR[19, 18] = 00  
  GPIOC->MODER &= ~(3 << (IOP_LED_PIN * 2));
  GPIOC->MODER |= (1 << (IOP_LED_PIN * 2)); // Output mode 01 
  GPIOC->OSPEEDR &= ~(3 << (IOP_LED_PIN * 2));
  GPIOC->OSPEEDR |= (3 << (IOP_LED_PIN * 2)); // High Speed 11 

  GPIOC->BSRR = 1 << IOP_LED_PIN;
  GPIOC->BRR = 1 << IOP_LED_PIN;
  GPIOC->BSRR = 1 << IOP_LED_PIN;
  GPIOC->BRR = 1 << IOP_LED_PIN;
  
  return 0;
}
