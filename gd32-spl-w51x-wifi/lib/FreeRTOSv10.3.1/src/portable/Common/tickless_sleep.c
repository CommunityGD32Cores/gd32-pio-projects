/*!
    \file    tickless_sleep.c
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

#include <stdint.h>
#include "wrapper_os.h"
#include "debug_print.h"
#include "bsp_wlan.h"
#include "wlan_intf.h"
#include "wifi_netlink.h"
#include "wakelock.h"
#include "tickless_sleep.h"

#if defined(PLATFORM_OS_FREERTOS)
#if ( configUSE_TICKLESS_IDLE != 0 )
static uint32_t total_sleep_time = 0;
extern uint32_t xMaximumPossibleSuppressedTicks;
/* maybe need wrap it as sys_task_step_tick later */
extern void vTaskStepTick(const uint32_t xTicksToJump);

void freertos_pre_sleep_processing(unsigned int *expected_idle_time)
{
	uint16_t sleep_time;
	struct time_rtc time_before_sleep;
	struct time_rtc time_after_sleep;
	uint32_t passed_time;
	uint32_t systick_cur_val;

	if (wifi_netlink_ps_get() == WIFI_PS_MODE_LPS_AND_CPU_DEEPSLEEP) {
		if (*expected_idle_time < xMaximumPossibleSuppressedTicks) {
			sleep_time = DEEP_SLEEP_MIN_TIME_MS;
		} else {
			sleep_time = DEEP_SLEEP_MAX_TIME_MS;
		}
		/* set expected_idle_time to 0 to use our sleep function and bypass freertos wfi sleep */
		*expected_idle_time = 0;

		rtc_32k_time_get(&time_before_sleep, 0);
		// DEBUGPRINT("time_before_sleep sec %d msec %d\r\n\n", time_before_sleep.tv_sec, time_before_sleep.tv_msec);

		systick_cur_val = *portNVIC_SYSTICK_CURRENT_VALUE;

		deep_sleep_enter(sleep_time);

		*portNVIC_SYSTICK_CURRENT_VALUE = systick_cur_val;
		rtc_32k_time_get(&time_after_sleep, 1);

		/* CPU wake up by interrupt and run in irc16M clock, so we need enable interrupt to change clock
		by func system_clock_config_nspe() in NSPE\WIFI_IOT\bsp\bsp_wlan.c */
		__asm volatile ("cpsie i" : : : "memory");
		__asm volatile ("dsb" ::: "memory");
		__asm volatile ("isb");

		// DEBUGPRINT("time_after_sleep sec %d msec %d\r\n", time_after_sleep.tv_sec, time_after_sleep.tv_msec);

		if (time_after_sleep.tv_sec >= time_before_sleep.tv_sec) {
			passed_time = (time_after_sleep.tv_sec * 1000 + time_after_sleep.tv_msec) -
					(time_before_sleep.tv_sec * 1000 + time_before_sleep.tv_msec);
		} else {
			passed_time = ((time_after_sleep.tv_sec + 60) * 1000 + time_after_sleep.tv_msec) -
					(time_before_sleep.tv_sec * 1000 + time_before_sleep.tv_msec);
		}

		// DEBUGPRINT("wakeup: sleep time = %d passed_time = %d\r\n", sleep_time, passed_time);

		vTaskStepTick(passed_time);
		total_sleep_time += passed_time;
    }
}

void freertos_post_sleep_processing(unsigned int *expected_idle_time)
{
	if (wifi_netlink_ps_get() == WIFI_PS_MODE_LPS_AND_CPU_DEEPSLEEP) {
		*expected_idle_time = 1 + ((*portNVIC_SYSTICK_CURRENT_VALUE) / (configCPU_CLOCK_HZ / OS_TICK_RATE_HZ));
	}
}

int freertos_ready_to_sleep(void)
{
	return sys_wakelock_status_get() == 0;
}
#endif  /* ( configUSE_TICKLESS_IDLE != 0 ) */
#else
/* TODO: other OS support */

#endif
