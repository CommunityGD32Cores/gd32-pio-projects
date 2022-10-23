/*!
    \file    mbl_trace.h
    \brief   Non-secure MBL trace file for GD32W51x WiFi SDK

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

#ifndef __MBL_TRACE_H__
#define __MBL_TRACE_H__

#include "stdint.h"
#include "stddef.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* Define Trace Level */
#define MBL_ALWAYS        1
#define MBL_ERR           2
#define MBL_WARN          3
#define MBL_INFO          4
#define MBL_DBG           5

extern int32_t mbl_trace_level;

#define MBL_ASSERT(expr)             \
    if ((expr) == 0) {               \
        printf("ASSERT: "#expr);     \
        for( ;; );                   \
    }

#define mbl_trace(level, fmt, arg...) do {  \
    if (level <= mbl_trace_level) {         \
        if (level == MBL_ERR) {             \
            printf("ERR: "fmt, ##arg);      \
        } else if (level == MBL_WARN) {     \
            printf("WARN: "fmt, ##arg);     \
        } else if (level == MBL_INFO) {     \
            printf("INFO: "fmt, ##arg);     \
        } else if (level == MBL_DBG) {      \
            printf("DBG: "fmt, ##arg);      \
        } else {                            \
            printf(fmt, ##arg);             \
        }                                   \
    }                                       \
} while(0);

#define mbl_print_data(title, data, count) do {\
    int i;                                     \
    printf("\r\n----------------------\r\n");  \
    printf("%s\r\n\t", title);                 \
    for (i = 0; i < count; i++) {              \
        if ((i > 0) && (i % 16 == 0))          \
            printf("\r\n\t");                  \
        printf("%02x ", *(data + i));          \
    }                                          \
    printf("\r\n");                            \
} while(0);

#endif  // __MBL_TRACE_H__
