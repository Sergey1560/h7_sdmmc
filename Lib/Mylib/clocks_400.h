#ifndef CLOCKS400_H
#define CLOCKS400_H

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
#define PLL_DIVN1 50

#define PLL_DIVP1 2
#define PLL_DIVQ1 16
#define PLL_DIVR1 2

#define PLL_FRACN1 0

/* PLL2 */
#define PLL_DIVN2 60

#define PLL_DIVP2 2
#define PLL_DIVQ2 20
#define PLL_DIVR2 4

#define PLL_FRACN2 0
/* PLL3 */
#define PLL_DIVN3 12

#define PLL_DIVP3 32
#define PLL_DIVQ3 4
#define PLL_DIVR3 6


#define PLL_FRACN3 0

#define RCC_D1CPRE 0


#define I2C_TIMINGS (uint32_t)0x10C0ECFF


/* Тактирование шин */
#define APB1_CLK 100000000UL
#define APB2_CLK 100000000UL

#define QSPI_CLK_SRC  2
#define FMC_CLK_SRC   2
#define FDCAN_CLK_SRC 2
#define SPI_CLK_SRC   2

#endif


/*
Частота:
SYSCLK 400
AXI 200
APB 100
APB_TIM 200

SPI1,2,3 -> PLL3P -> 6
USART1,6 -> PCLK2 -> 100

I2C1,2,3 -> PCLK1 -> 100

FMC -> PLL2R -> 240

QUADSPI -> PLL2R -> 240

LTDC -> DIVR3 -> 32

USB -> PLL3Q -> 48

SDMMC -> PLL1Q -> 50

FDCAN -> PLL2Q -> 48
*/