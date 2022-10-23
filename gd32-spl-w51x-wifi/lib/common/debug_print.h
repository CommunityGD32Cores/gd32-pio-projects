/*!
    \file    debug_print.h
    \brief   Debug print for GD32W51x WiFi SDK

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#ifndef __DEBUG_PRINT_H
#define __DEBUG_PRINT_H

#include "app_cfg.h"
#include <stdio.h>

#define CONFIG_DEBUG_PRINT_ENABLE
#ifdef CONFIG_DEBUG_PRINT_ENABLE
// #define CONFIG_PRINT_IN_SEQUENCE
#endif

#define ERROR_LEVEL             1
#define WARNING_LEVEL           2
#define INFO_LEVEL              3

#define ETHIF_LEVEL             INFO_LEVEL
#define MAIN_LEVEL              INFO_LEVEL
#define HTTPD_LEVEL             INFO_LEVEL
#define TCP_OUT_LEVEL           INFO_LEVEL
#define SYS_ARCH_LEVEL          INFO_LEVEL
#define WIFI_NETLINK_LEVEL      INFO_LEVEL

#ifdef CONFIG_ATCMD
#undef WIFI_NETLINK_LEVEL
#define WIFI_NETLINK_LEVEL      INFO_LEVEL
#endif

#define MAC_ARG(a)    (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MAC_FMT       "%02x:%02x:%02x:%02x:%02x:%02x"
#define IP_FMT        "%d.%d.%d.%d"
#define IP_ARG(a)     ((a) & 0xFF), (((a) >> 8) & 0xFF), (((a) >> 16) & 0xFF), ((a) >> 24)

int buffer_print(unsigned long addr, void *data, unsigned long width, unsigned long count, unsigned long linelen);
void buffer_dump(char *title, unsigned char *mem, int mem_size);
int str2hex(char *input, int input_len, unsigned char *output, int output_len);
int co_printf(const char *format, ...);

#define DEBUGPRINT    co_printf


#ifdef CONFIG_DEBUG_PRINT_ENABLE

#define _CONCAT(a, b) a##b
#define CONCAT(a, b) _CONCAT(a, b)
#define _NAME(a) #a
#define NAME(a) _NAME(a)


#define WHERESTR  "[module %s, line %d]: "
#define WHEREARG  NAME(MOUDLE), __LINE__

#define CO_DEBUG_PRINTF(level,...)  do{ \
                                        /*DEBUGPRINT( level WHERESTR , WHEREARG); */\
                                        DEBUGPRINT( __VA_ARGS__);\
                                    }while(0);


#if ( CONCAT(MOUDLE, _LEVEL) >= INFO_LEVEL )
#define DEBUG_INFO(...) CO_DEBUG_PRINTF("INF", __VA_ARGS__)
#else
#define DEBUG_INFO(...)
#endif


#if ( CONCAT(MOUDLE, _LEVEL) >= WARNING_LEVEL )
#define DEBUG_WARNING(...)  CO_DEBUG_PRINTF("WAR", __VA_ARGS__)
#else
#define DEBUG_WARNING(...)
#endif


#if ( CONCAT(MOUDLE, _LEVEL) >= ERROR_LEVEL )
#define DEBUG_ERROR(...) CO_DEBUG_PRINTF("ERR", __VA_ARGS__)
#else
#define DEBUG_ERROR(...)
#endif

#if ( CONCAT(MOUDLE, _LEVEL) >= INFO_LEVEL )
#define DEBUG_DUMP_DATA(memaddr, memlen) do{ \
                                            DEBUGPRINT("\r\n"  WHERESTR , WHEREARG);\
                                            DEBUGPRINT("\r\n");\
                                            buffer_dump("", memaddr,memlen);\
                                           }while(0);

#else
#define DEBUG_DUMP_DATA(...)
#endif

#else  /*  CONFIG_DEBUG_PRINT_ENABLE */

#define DEBUG_INFO(...)
#define DEBUG_WARNING(...)
#define DEBUG_ERROR(...)
#define DEBUG_DUMP_DATA(...)
#define DEBUGPRINT(...)

#endif  /*  CONFIG_DEBUG_PRINT_ENABLE */

#ifndef DEBUG_ASSERT
#define DEBUG_ASSERT(expr)  do {\
                             if (!(expr))\
                                 {DEBUGPRINT("%s:%u ASSERT: "#expr"\r\n", __func__, __LINE__);}\
                             } while(0)
#endif

#endif /* __DEBUG_PRINT_H */
