#ifndef COMDEFS_H
#define COMDEFS_H
#include "stm32h7xx.h"
#include "log.h"
#include "dwt.h"
#include "mem2mem.h"

#define ENABLE_DCACHE 1

#define CCMRAM __attribute__((section(".ccmram")))
#define RAM_D1 __attribute__((section(".ram_d1")))
#define RAM_D2 __attribute__((section(".ram_d2")))
#define ITCM __attribute__((section(".itcm")))
#define DTCM __attribute__((section(".dtcm")))

#define ALGN4 __attribute__ ((aligned (4)))
#define ALGN8 __attribute__ ((aligned (8)))

/* GPIO SPEED */
#define S_LOW 0
#define S_MED 1
#define S_HI  2
#define S_VH  3


#ifndef GPIO_AFRH_AFRH8
#define GPIO_AFRH_AFRH8_Pos              (0U)                                  
#define GPIO_AFRH_AFRH8_Msk              (0xFU << GPIO_AFRH_AFRH0_Pos)         /*!< 0x0000000F */
#define GPIO_AFRH_AFRH8                  GPIO_AFRH_AFRH0_Msk                   
#define GPIO_AFRH_AFRH9_Pos              (4U)                                  
#define GPIO_AFRH_AFRH9_Msk              (0xFU << GPIO_AFRH_AFRH1_Pos)         /*!< 0x000000F0 */
#define GPIO_AFRH_AFRH9                  GPIO_AFRH_AFRH1_Msk                   
#define GPIO_AFRH_AFRH10_Pos              (8U)                                  
#define GPIO_AFRH_AFRH10_Msk              (0xFU << GPIO_AFRH_AFRH2_Pos)         /*!< 0x00000F00 */
#define GPIO_AFRH_AFRH10                  GPIO_AFRH_AFRH2_Msk                   
#define GPIO_AFRH_AFRH11_Pos              (12U)                                 
#define GPIO_AFRH_AFRH11_Msk              (0xFU << GPIO_AFRH_AFRH3_Pos)         /*!< 0x0000F000 */
#define GPIO_AFRH_AFRH11                  GPIO_AFRH_AFRH3_Msk                   
#define GPIO_AFRH_AFRH12_Pos              (16U)                                 
#define GPIO_AFRH_AFRH12_Msk              (0xFU << GPIO_AFRH_AFRH4_Pos)         /*!< 0x000F0000 */
#define GPIO_AFRH_AFRH12                  GPIO_AFRH_AFRH4_Msk                   
#define GPIO_AFRH_AFRH13_Pos              (20U)                                 
#define GPIO_AFRH_AFRH13_Msk              (0xFU << GPIO_AFRH_AFRH5_Pos)         /*!< 0x00F00000 */
#define GPIO_AFRH_AFRH13                  GPIO_AFRH_AFRH5_Msk                   
#define GPIO_AFRH_AFRH14_Pos              (24U)                                 
#define GPIO_AFRH_AFRH14_Msk              (0xFU << GPIO_AFRH_AFRH6_Pos)         /*!< 0x0F000000 */
#define GPIO_AFRH_AFRH14                  GPIO_AFRH_AFRH6_Msk                   
#define GPIO_AFRH_AFRH15_Pos              (28U)                                 
#define GPIO_AFRH_AFRH15_Msk              (0xFU << GPIO_AFRH_AFRH7_Pos)         /*!< 0xF0000000 */
#define GPIO_AFRH_AFRH15                  GPIO_AFRH_AFRH7_Msk                   
#endif


#endif