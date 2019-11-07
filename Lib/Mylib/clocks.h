#ifndef CLOCKS_H
#define CLOCKS_H

#include "common_defs.h"

#define STM32H743

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
#define PLL_DIVR1 2
#define PLL_DIVQ1 20
#define PLL_DIVP1 2
#define PLL_DIVN1 60
#define PLL_FRACN1 0
/* PLL2 */
#define PLL_DIVR2 1
#define PLL_DIVQ2 1
#define PLL_DIVP2 2
#define PLL_DIVN2 9
#define PLL_FRACN2 3072
/* PLL3 */
#define PLL_DIVR3 3
#define PLL_DIVQ3 2
#define PLL_DIVP3 2
#define PLL_DIVN3 6
#define PLL_FRACN3 0

#define RCC_D1CPRE 0


/* Тактирование шин */
#define APB1_CLK 120000000UL
#define APB2_CLK 120000000UL

/* Тактирование таймеров */
#define APB1_TIMER_CLK 2*APB1_CLK
#define APB2_TIMER_CLK 2*APB2_CLK


#define TIM2_CLK APB1_TIMER_CLK
#define TIM3_CLK APB1_TIMER_CLK
#define TIM4_CLK APB1_TIMER_CLK
#define TIM5_CLK APB1_TIMER_CLK
#define TIM6_CLK APB1_TIMER_CLK
#define TIM7_CLK APB1_TIMER_CLK
#define TIM12_CLK APB1_TIMER_CLK
#define TIM13_CLK APB1_TIMER_CLK
#define TIM14_CLK APB1_TIMER_CLK

#define TIM1_CLK APB2_TIMER_CLK
#define TIM8_CLK APB2_TIMER_CLK
#define TIM15_CLK APB2_TIMER_CLK
#define TIM16_CLK APB2_TIMER_CLK
#define TIM17_CLK APB2_TIMER_CLK


#endif