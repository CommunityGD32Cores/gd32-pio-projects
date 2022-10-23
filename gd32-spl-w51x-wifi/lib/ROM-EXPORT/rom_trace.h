/*!
    \file    rom_trace.h
    \brief   Rom trace file for GD32W51x WiFi SDK

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

#ifndef __IBL_TRACE_H__
#define __IBL_TRACE_H__

/* Define Trace Level */
#define ROM_OFF			0
#define ROM_ALWAYS		1
#define ROM_ERR			2
#define ROM_WARN		3
#define ROM_INFO		4
#define ROM_DBG			5

extern int32_t trace_level;

int rom_printf(const char *format, ...);
int rom_trace_ex(uint32_t level, const char *fmt, ...);
int rom_snprintf(char *out, uint32_t out_sz, const char *format, ...);
int rom_print_data(int32_t level, char *title, uint8_t *data, uint32_t count);

#define ROM_ASSERT(expr)							\
	if ((expr) == 0) {								\
		rom_printf("ASSERT: "#expr);					\
		for( ;; );									\
	}

#if 1
#define rom_trace rom_trace_ex
#else
#define rom_trace(level, fmt, arg...) do {\
	if (level <= trace_level) {\
		if (level == ROM_ERR) {\
			rom_printf("ERR: "fmt, ##arg);\
		} else if (level == ROM_WARN) {\
			rom_printf("WARN: "fmt, ##arg);\
		} else if (level == ROM_INFO) {\
			rom_printf("INFO: "fmt, ##arg);\
		} else if (level == ROM_DBG) {\
			rom_printf("DBG: "fmt, ##arg);\
		} else {\
			rom_printf(fmt, ##arg);\
		}\
	}\
} while(0);
#endif
#endif  // __IBL_TRACE_H__

