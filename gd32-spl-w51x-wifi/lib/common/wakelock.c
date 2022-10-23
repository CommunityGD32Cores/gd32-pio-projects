/*!
    \file    wakelock.c
    \brief   OS wake lock for GD32W51x WiFi SDK

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

#include "wakelock.h"

static uint32_t wake_lock = 0;

/*!
    \brief      acquire wakelock and do not enter power saving mode
    \param[in]  lock_id: lock id
    \param[out] none
    \retval     none
*/
void sys_wakelock_acquire(uint32_t lock_id)
{
    wake_lock |= (0x01 << lock_id);
}

/*!
    \brief      release wakelock and allow access to power saving mode
    \param[in]  lock_id: lock id
    \param[out] none
    \retval     none
*/
void sys_wakelock_release(uint32_t lock_id)
{
    wake_lock &= ~(0x01 << lock_id);
}

/*!
    \brief      get wakelock status
    \param[in]  none
    \param[out] none
    \retval     none
*/
uint32_t sys_wakelock_status_get(void)
{
    return wake_lock;
}

/*!
    \brief      acquire wakelock by systick
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_lock_acquire()

{
    sys_wakelock_acquire(LOCK_ID_SYSTICK);
}

/*!
    \brief      release wakelock by systick
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_lock_release()
{
    sys_wakelock_release(LOCK_ID_SYSTICK);
}
