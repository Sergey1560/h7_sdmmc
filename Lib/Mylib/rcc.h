#ifndef RCC_H
#define RCC_H
#include "stm32h7xx.h"
#include "clocks.h"

void RCC_init(void);
void system_reset(void);
#endif