#ifndef HARD_H
#define HARD_H
#include "stm32h7xx.h"
#include <stdio.h>
#include <string.h>

#include <stdbool.h>

void hard_fault_handl_c (unsigned int * hardfault_args);
bool cpu_check_address(volatile const char *address);

#endif