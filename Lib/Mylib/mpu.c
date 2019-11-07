#include "mpu.h"


void MPU_Disable(void){
  __DMB();
  /* Disable fault exceptions */
  SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;
  /* Disable the MPU and clear the control register*/
  MPU->CTRL = 0;
}

void MPU_Enable(uint32_t MPU_Control){
  /* Enable the MPU */
  MPU->CTRL = MPU_Control | MPU_CTRL_ENABLE_Msk;
  /* Enable fault exceptions */
  SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
  /* Ensure MPU setting take effects */
  __DSB();
  __ISB();
}

void MPU_Config(void){

	/* Disables the MPU */
	MPU_Disable();
	/** Initializes and configures the Region and the memory to be protected 
	 */

	/* Set the Region number */
	MPU->RNR = MPU_REGION_NUMBER0;


	MPU->RBAR = 0xD0000000;
	MPU->RASR = ((uint32_t)MPU_INSTRUCTION_ACCESS_ENABLE << MPU_RASR_XN_Pos)   |
				((uint32_t)MPU_REGION_FULL_ACCESS  << MPU_RASR_AP_Pos)   |
				((uint32_t)MPU_TEX_LEVEL0          << MPU_RASR_TEX_Pos)  |
				((uint32_t)MPU_ACCESS_NOT_SHAREABLE << MPU_RASR_S_Pos)    |
				((uint32_t)MPU_ACCESS_CACHEABLE    << MPU_RASR_C_Pos)    |
				((uint32_t)MPU_ACCESS_NOT_BUFFERABLE << MPU_RASR_B_Pos)    |
				((uint32_t)0x0				       << MPU_RASR_SRD_Pos)  |
				((uint32_t)MPU_REGION_SIZE_8MB     << MPU_RASR_SIZE_Pos) |
				((uint32_t)1                       << MPU_RASR_ENABLE_Pos);
/*
	MPU->RNR = MPU_REGION_NUMBER1;
	MPU->RBAR = (uint32_t)&letter;
	MPU->RASR = ((uint32_t)MPU_INSTRUCTION_ACCESS_ENABLE << MPU_RASR_XN_Pos)   |
				((uint32_t)MPU_REGION_FULL_ACCESS  << MPU_RASR_AP_Pos)   |
				((uint32_t)MPU_TEX_LEVEL0          << MPU_RASR_TEX_Pos)  |
				((uint32_t)MPU_ACCESS_NOT_SHAREABLE << MPU_RASR_S_Pos)    |
				((uint32_t)MPU_ACCESS_NOT_CACHEABLE    << MPU_RASR_C_Pos)    |
				((uint32_t)MPU_ACCESS_NOT_BUFFERABLE << MPU_RASR_B_Pos)    |
				((uint32_t)0x0				       << MPU_RASR_SRD_Pos)  |
				((uint32_t)MPU_REGION_SIZE_32KB     << MPU_RASR_SIZE_Pos) |
				((uint32_t)1                       << MPU_RASR_ENABLE_Pos);
*/
  /* Enables the MPU */
  MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
