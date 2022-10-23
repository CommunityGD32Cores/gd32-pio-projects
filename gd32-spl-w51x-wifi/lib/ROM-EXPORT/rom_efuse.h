/*!
    \file    rom_efuse.h
    \brief   Rom efuse interface for GD32W51x WiFi SDK

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

#ifndef __EFUSE_H__
#define __EFUSE_H__


#if defined(PLATFORM_MSP_AN521)
#define EFUSE_ROTPK_SZ		    32		/* 256 bits */
#define EFUSE_HUK_SZ		    16		/* 128 bits */
#define EFUSE_OTP_SZ		    16		/* 128 bits */

#define GET_IBL_OPT()           (0)

int efuse_get_rotpk_inner(uint8_t *rotpk);
int efuse_get_huk_inner(uint8_t *huk);

#elif defined(PLATFORM_GDM32)
#define EFUSE_ROTPK_SZ		    EFUSE_WD_ROTPK
#define EFUSE_HUK_SZ		    EFUSE_WD_HUK
#define EFUSE_OTP_SZ		    EFUSE_WD_US_DATA

#define IS_READ_PROTECT_1()     (OBSTAT_TZEN() && OBSTAT_RDP_1())
#define IS_READ_PROTECT()       (OBSTAT_TZEN() && (OBSTAT_RDP_0_5() || OBSTAT_RDP_1()))
#define IS_FW_CRYPT()           ((efuse_get_usctl() & EFUSE_USCTL_AESEN) ? 1 : 0)
#define GET_IBL_OPT()           (efuse_get_tzctl() >> 6)

int efuse_get_rotpk_inner(uint8_t *rotpk);
int efuse_get_dbg_pwd_inner(uint8_t *pwd);
int efuse_get_huk_inner(uint8_t *huk);
void efuse_dump_inner(void);
#endif
int is_key_valid(uint8_t *key, size_t key_sz);

#ifdef ROM_SELF_TEST
void efuse_self_test(void);
#endif
#endif  /* __EFUSE_H__ */
