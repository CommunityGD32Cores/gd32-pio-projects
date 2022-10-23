/*!
    \file    tickless_sleep.h
    \brief   Tickless sleep function for GD32W51x WiFi SDK

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

#ifndef TICKLESS_SLEEP_H
#define TICKLESS_SLEEP_H

#define DEEP_SLEEP_MIN_TIME_MS	1000
#define DEEP_SLEEP_MAX_TIME_MS	10000

#ifndef portNVIC_SYSTICK_CURRENT_VALUE
#define portNVIC_SYSTICK_CURRENT_VALUE	((volatile uint32_t *)0xe000e018)
#endif
#ifndef portSY_FULL_READ_WRITE
#define portSY_FULL_READ_WRITE			(15)
#endif

#if defined(PLATFORM_OS_FREERTOS)
void freertos_pre_sleep_processing(unsigned int *expected_idle_time);
void freertos_post_sleep_processing(unsigned int *expected_idle_time);
int freertos_ready_to_sleep(void);
#endif

#endif

