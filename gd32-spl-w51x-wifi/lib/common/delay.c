/*!
    \file    delay.c
    \brief   Delay function for GD32W51x WiFi SDK

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

#include "app_cfg.h"
#include "app_type.h"
#include "bsp_inc.h"
#include "wrapper_os.h"

static uint8_t fac_us = 0;

/*!
    \brief      initialize systick delay parameter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_delay_init(void)
{
    fac_us = SystemCoreClock / 1000000;
/*
    reload = SystemCoreClock/OSCfg_TickRate_Hz;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->LOAD = reload;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
*/
}

void systick_reconfig(void)
{
    uint32_t reload;

    fac_us = SystemCoreClock / 1000000;
    reload = SystemCoreClock / OS_TICK_RATE_HZ;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->LOAD = reload;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/*!
    \brief      get systick ticks
    \param[in]  none
    \param[out] none
    \retval     none
*/
uint32_t systick_ticks_get(void)
{
    return ( SysTick->LOAD - SysTick->VAL );
}

/*!
    \brief      get systick load value
    \param[in]  none
    \param[out] none
    \retval     none
*/
uint32_t systick_load_get(void)
{
    return SysTick->LOAD;
}

/*!
    \brief      delay a time in microseconds
    \param[in]  nus: count in microseconds
    \param[out] none
    \retval     none
*/
void systick_udelay(uint32_t nus)
{
    uint32_t ticks;
    uint32_t t_old, t_now;
    uint32_t t_cnt = 0;
    uint32_t reload = SysTick->LOAD;

    ticks = nus * fac_us;
    t_cnt = 0;

    t_old = SysTick->VAL;
    while (1) {
        t_now = SysTick->VAL;
        if (t_now != t_old) {
            if (t_now < t_old) {
                t_cnt += t_old - t_now;
            } else {
                t_cnt += reload - t_now + t_old;
            }
            t_old = t_now;
            if (t_cnt >= ticks)
                break;
        }
    };
}
