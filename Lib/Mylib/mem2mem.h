#ifndef MEM2MEM_H
#define MEM2MEM_H
#include "common_defs.h"

#define DMA_MEM_INCR (uint32_t) ((2 << DMA_SxCR_DIR_Pos)| \
                                  DMA_SxCR_PINC|\
                                  DMA_SxCR_MINC|\
                                  DMA_SxCR_MSIZE_1|\
                                  DMA_SxCR_PSIZE_1)

#define DMA_MEM_STAT (uint32_t) ((2 << DMA_SxCR_DIR_Pos)| \
                                  DMA_SxCR_MINC|\
                                  DMA_SxCR_MSIZE_1|\
                                  DMA_SxCR_PSIZE_1)

#define DMA_CLEAR  		        ((uint32_t) DMA_LIFCR_CDMEIF1| DMA_LIFCR_CFEIF1 | DMA_LIFCR_CHTIF1  |DMA_LIFCR_CTCIF1  |DMA_LIFCR_CTEIF1)

void dma_mem2mem(uint32_t *src, uint32_t *dst, uint32_t len, uint8_t inrc);

#endif