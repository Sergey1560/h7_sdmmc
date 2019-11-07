#include "mem2mem.h"


void dma_mem2mem(uint32_t *src, uint32_t *dst, uint32_t len, uint8_t inrc){
  static uint8_t first_run=1;

  if(first_run > 0){
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    first_run=0; 
  }
  
  uint32_t len32=len/4;

	DMA2_Stream1->CR = 0;
	if(inrc){
		DMA2_Stream1->CR = DMA_MEM_INCR;
	}else{
		DMA2_Stream1->CR = DMA_MEM_STAT;
	}

	while(len32){
		if(len32 > 0xFFFC){
			DMA2_Stream1->NDTR = (uint16_t)0xFFFC;
			len32 = len32 - 0xFFFC;
		}else{
			DMA2_Stream1->NDTR = (uint16_t)len32;
			len32=0;
		};
		DMA2_Stream1->PAR = (uint32_t)src;
		DMA2_Stream1->M0AR = (uint32_t)dst;
        #ifdef ENABLE_DCACHE 
		SCB_CleanDCache();
		#endif
		DMA2_Stream1->CR |=  DMA_SxCR_EN;
        #ifdef ENABLE_DCACHE 
		SCB_InvalidateDCache();
		#endif
		
    while(!(DMA2 -> LISR & (DMA_LISR_TCIF1))){asm volatile("nop");};
	
    dst+=0xFFFC;
	if(inrc){
		src+=0xFFFC;
	};
	DMA2->LIFCR = DMA_CLEAR;
	}

};
