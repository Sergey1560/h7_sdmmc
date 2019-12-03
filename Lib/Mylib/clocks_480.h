#ifndef CLOCKS480_H
#define CLOCKS480_H

#define RCC_HSE 16000000

/* Источник PLL */
#define PLL_HSE 2
#define PLL_HSI 0
#define PLL_SRC PLL_HSE

/* Основные настройки тактирования */
#define RCC_DIVM1 1
#define RCC_DIVM2 1
#define RCC_DIVM3 1

/* PLL1 */
#define PLL_DIVN1 60
#define PLL_DIVP1 2
#define PLL_DIVQ1 20
#define PLL_DIVR1 2
#define PLL_FRACN1 0
/* PLL2 */
#define PLL_DIVN2 20
#define PLL_DIVP2 2
#define PLL_DIVQ2 1
#define PLL_DIVR2 1
#define PLL_FRACN2 0
/* PLL3 */
#define PLL_DIVN3 12
#define PLL_DIVP3 32
#define PLL_DIVQ3 2
#define PLL_DIVR3 6
#define PLL_FRACN3 0

#define RCC_D1CPRE 0

#define I2C_TIMINGS (uint32_t)0x20A09DEB

/* Тактирование шин */
#define APB1_CLK 120000000UL
#define APB2_CLK 120000000UL

/* Источники тактирования для периферии */
#define QSPI_CLK_SRC  0
#define FMC_CLK_SRC   0
#define FDCAN_CLK_SRC 1
#define SPI_CLK_SRC   2
#endif

/*
Частота:
SYSCLK 480
AXI 240
APB 120
APB_TIM 240

SPI1,2,3 -> PLL3P -> 6
USART1,6 -> PCLK2 -> 120

I2C1,2,3 -> PCLK1 -> 120

FMC -> HCLK3 -> 240

QUADSPI -> HCLK3 -> 240

LTDC -> DIVR3 -> 32

USB -> PLL1Q -> 48

SDMMC -> PLL1Q -> 48

FDCAN -> PLL1Q -> 48
*/