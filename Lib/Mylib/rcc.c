#include "rcc.h"

void RCC_init(void){
    /** Supply configuration update enable */
    PWR->CR3 &= ~PWR_CR3_SCUEN;
    /** Configure the main internal regulator output voltage  */
    PWR->D3CR |= PWR_D3CR_VOS_1 | PWR_D3CR_VOS_0;

    while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY){};
    /** Initializes the CPU, AHB and APB busses clocks  */
    
    RCC->CR |= RCC_CR_HSEON;
    while((RCC->CR & RCC_CR_HSERDY)==0){};

    RCC->PLLCKSELR &= ~(RCC_PLLCKSELR_PLLSRC|RCC_PLLCKSELR_DIVM1||RCC_PLLCKSELR_DIVM2||RCC_PLLCKSELR_DIVM3);
    RCC->PLLCKSELR |= (PLL_SRC << RCC_PLLCKSELR_PLLSRC_Pos)|\
                      (RCC_DIVM1 << RCC_PLLCKSELR_DIVM1_Pos)|\
                      (RCC_DIVM2 << RCC_PLLCKSELR_DIVM2_Pos)|\
                      (RCC_DIVM3 << RCC_PLLCKSELR_DIVM3_Pos);
    /* PLL1 */    
    RCC->PLL1DIVR &= ~(RCC_PLL1DIVR_P1_Pos|RCC_PLL1DIVR_N1|RCC_PLL1DIVR_R1|RCC_PLL1DIVR_Q1);
    RCC->PLL1DIVR |= ((PLL_DIVP1 -1) << RCC_PLL1DIVR_P1_Pos)|\
                     ((PLL_DIVN1 -1) << RCC_PLL1DIVR_N1_Pos)|\
                     ((PLL_DIVR1 -1) << RCC_PLL1DIVR_R1_Pos)|\
                     ((PLL_DIVQ1 -1) << RCC_PLL1DIVR_Q1_Pos);
    RCC -> PLL1FRACR = PLL_FRACN1; 
    /* PLL2 */    
    RCC->PLL2DIVR &= ~(RCC_PLL2DIVR_P2_Pos|RCC_PLL2DIVR_N2|RCC_PLL2DIVR_R2|RCC_PLL2DIVR_Q2);
    RCC->PLL2DIVR |= ((PLL_DIVP2 -1) << RCC_PLL2DIVR_P2_Pos)|\
                     ((PLL_DIVN2 -1) << RCC_PLL2DIVR_N2_Pos)|\
                     ((PLL_DIVR2 -1) << RCC_PLL2DIVR_R2_Pos)|\
                     ((PLL_DIVQ2 -1) << RCC_PLL2DIVR_Q2_Pos);
    RCC -> PLL2FRACR = PLL_FRACN2; 
    /* PLL3 */    
    RCC->PLL3DIVR &= ~(RCC_PLL3DIVR_P3_Pos|RCC_PLL3DIVR_N3|RCC_PLL3DIVR_R3|RCC_PLL3DIVR_Q3);
    RCC->PLL3DIVR |= ((PLL_DIVP3 -1) << RCC_PLL3DIVR_P3_Pos)|\
                     ((PLL_DIVN3 -1) << RCC_PLL3DIVR_N3_Pos)|\
                     ((PLL_DIVR3 -1) << RCC_PLL3DIVR_R3_Pos)|\
                     ((PLL_DIVQ3 -1) << RCC_PLL3DIVR_Q3_Pos);
    RCC -> PLL3FRACR = PLL_FRACN3; 

    //PLL input freq range 8-16Mhz
    RCC->PLLCFGR |= RCC_PLLCFGR_PLL1RGE_3|RCC_PLLCFGR_PLL2RGE_3|RCC_PLLCFGR_PLL3RGE_3;
    
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLL1VCOSEL|RCC_PLLCFGR_PLL2VCOSEL|RCC_PLLCFGR_PLL3VCOSEL);
    
    RCC->PLLCFGR |= (RCC_PLLCFGR_DIVP1EN|\
                    RCC_PLLCFGR_DIVQ1EN|\
                    RCC_PLLCFGR_DIVR1EN|\
                    RCC_PLLCFGR_PLL1FRACEN|\
                    RCC_PLLCFGR_DIVP2EN|\
                    RCC_PLLCFGR_DIVQ2EN|\
                    RCC_PLLCFGR_DIVR2EN|\
                    RCC_PLLCFGR_PLL2FRACEN|\
                    RCC_PLLCFGR_DIVP3EN|\
                    RCC_PLLCFGR_DIVQ3EN|\
                    RCC_PLLCFGR_DIVR3EN|\
                    RCC_PLLCFGR_PLL3FRACEN);
    //Enable PLL
    RCC->CR |= RCC_CR_PLL1ON;
    while((RCC->CR & RCC_CR_PLL1RDY)==0){};

    RCC->CR |= RCC_CR_PLL2ON;
    while((RCC->CR & RCC_CR_PLL2RDY)==0){};

    RCC->CR |= RCC_CR_PLL3ON;
    while((RCC->CR & RCC_CR_PLL3RDY)==0){};


    /** Initializes the CPU, AHB and APB busses clocks */
    RCC->D1CFGR &= ~(RCC_D1CFGR_D1CPRE|RCC_D1CFGR_HPRE|RCC_D1CFGR_D1PPRE);
    RCC->D1CFGR |=  (RCC_D1CPRE << RCC_D1CFGR_D1CPRE_Pos)| RCC_D1CFGR_HPRE_DIV2 | RCC_D1CFGR_D1PPRE_DIV2;


    RCC->D2CFGR &= ~(RCC_D2CFGR_D2PPRE1|RCC_D2CFGR_D2PPRE2);

    RCC->D2CFGR |= RCC_D2CFGR_D2PPRE1_DIV2|RCC_D2CFGR_D2PPRE2_DIV2;
    
    RCC->D3CFGR &= ~RCC_D3CFGR_D3PPRE;
    RCC->D3CFGR |= RCC_D3CFGR_D3PPRE_DIV2;


    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL1;
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL1) {};
    

	SystemCoreClockUpdate();
    INFO("RCC init, SystemCoreClock %d",SystemCoreClock);
}

void system_reset(void){
	SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEYSTAT_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
	__DSB();                                                                                 
  	while(1);  
}

