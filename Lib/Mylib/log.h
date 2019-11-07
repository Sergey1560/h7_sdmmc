#ifndef LOG_H
#define LOG_H
#include "dwt.h"
#include "SEGGER_RTT.h"

//#define DEBUG_MSG
#define INFO_MSG
#define WARNING_MSG
#define ERROR_MSG


#ifdef COLOR_LOG_RTT
    #define DEBUG(fmt, args...) SEGGER_RTT_printf(0,"%s%s %-20s:%-4d: " fmt "%s\r\n", RTT_CTRL_TEXT_WHITE, GetUpTimeASCII(), __func__, __LINE__, ## args, RTT_CTRL_RESET)
    #define INFO(fmt, args...)  SEGGER_RTT_printf(0,"%s%s %-20s:%-4d: " fmt "%s\r\n", RTT_CTRL_TEXT_BRIGHT_GREEN, GetUpTimeASCII(), __func__, __LINE__, ## args, RTT_CTRL_RESET)
    #define WARNING(fmt, args...) SEGGER_RTT_printf(0,"%s%s %-20s:%-4d: " fmt "%s\r\n", RTT_CTRL_TEXT_BRIGHT_YELLOW, GetUpTimeASCII(), __func__, __LINE__, ## args, RTT_CTRL_RESET)
    #define ERROR(fmt, args...) SEGGER_RTT_printf(0,"%s%s %-20s:%-4d: " fmt "%s\r\n", RTT_CTRL_TEXT_BRIGHT_RED, GetUpTimeASCII(), __func__, __LINE__, ## args, RTT_CTRL_RESET)
#else
    #ifdef DEBUG_MSG
        #define DEBUG(fmt, args...) SEGGER_RTT_printf(0,"DEBUG %-20s:%-4d [%d]:" fmt "\r\n", __func__, __LINE__,dwt_get_tick_in_sec(), ## args)
    #else
        #define DEBUG(fmt, args...)
    #endif
    #ifdef INFO_MSG
        #define INFO(fmt, args...)  SEGGER_RTT_printf(0,"INFO %-20s:%-4d [%d]:" fmt "\r\n", __func__, __LINE__,dwt_get_tick_in_sec(), ## args)
    #else
        #define INFO(fmt, args...)
    #endif
    #ifdef WARNING_MSG
        #define WARNING(fmt, args...) SEGGER_RTT_printf(0,"WARN %-20s:%-4d [%d]:" fmt "\r\n",  __func__, __LINE__,dwt_get_tick_in_sec(), ## args)
    #else
        #define WARNING(fmt, args...)
    #endif
    #ifdef ERROR_MSG
        #define ERROR(fmt, args...) SEGGER_RTT_printf(0,"ERR %-20s:%-4d [%d]:" fmt "\r\n",  __func__, __LINE__,dwt_get_tick_in_sec(),## args)
    #else
        #define ERROR(fmt, args...)
    #endif
#endif

#endif
