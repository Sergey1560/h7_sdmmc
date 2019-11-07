#include "dwt.h"


void dwt_init(void){

	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

}


uint32_t dwt_get_tick(void){
    return DWT->CYCCNT;
}

uint32_t dwt_get_tick_in_sec(void){
    return DWT->CYCCNT/(SystemCoreClock/1000);
}

uint32_t dwt_get_diff_tick(uint32_t tick){
    uint32_t result=0;

    if(DWT->CYCCNT > tick){
        result=DWT->CYCCNT-tick;
    }else{
        result=(uint32_t)(0xFFFFFFFF-tick)+DWT->CYCCNT;
    };

    return result;
}

uint32_t dwt_get_diff_sec(uint32_t tick){

    return dwt_get_diff_tick(tick)/(SystemCoreClock/1000);
}


