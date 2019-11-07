#ifndef DWT_H
#define DWT_H

#include "common_defs.h"

void dwt_init(void);
uint32_t dwt_get_tick(void);
uint32_t dwt_get_diff_tick(uint32_t tick);
uint32_t dwt_get_diff_sec(uint32_t tick);
uint32_t dwt_get_tick_in_sec(void);
#endif
