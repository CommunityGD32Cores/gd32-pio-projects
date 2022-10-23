/*!
    \file    init_rom.c
    \brief   Rom init function for GD32W51x WiFi SDK

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

#include "wrapper_os.h"

#if defined(CONFIG_TZ_ENABLED)
#include "rom_export.h"
#include "mbl_nsc_api.h"
#include "ssl_tz.h"
#include "init_rom.h"  /* must be included after ssl_tz.h */
#include "ns_interface.h"
#else  /* CONFIG_TZ_ENABLED */
#include "rom_export_mbedtls.h"
#include "rom_api.h"
#include "mbl_api.h"
#include "init_rom.h"
#include "trng.h"
struct rom_api_s_t *p_rom_api_s = (struct rom_api_s_t *)0x0FF4F000;
struct mbl_api_t *p_mbl_api = (struct mbl_api_t *)MBL_API_START;
#endif  /* CONFIG_TZ_ENABLED */


extern os_mutex_t hw_pka_lock;
extern os_mutex_t hw_cryp_lock;
extern os_mutex_t hw_hash_lock;

/*!
    \brief      get system mutex function
    \param[in]  type: mutex type(MUTEX_TYPE_PKA, MUTEX_TYPE_CRYP or MUTEX_TYPE_HASH)
    \param[out] none
    \retval     none
*/
void sys_mutex_get_func(int type)
{
    switch(type)
    {
        case MUTEX_TYPE_PKA:
            sys_mutex_get(&hw_pka_lock);
            break;
        case MUTEX_TYPE_CRYP:
            sys_mutex_get(&hw_cryp_lock);
            break;
        case MUTEX_TYPE_HASH:
            sys_mutex_get(&hw_hash_lock);
            break;
        default:
            break;
    }
}

/*!
    \brief      put system mutex
    \param[in]  type: mutex type(MUTEX_TYPE_PKA, MUTEX_TYPE_CRYP or MUTEX_TYPE_HASH)
    \param[out] none
    \retval     none
*/
void sys_mutex_put_func(int type)
{
    switch(type)
    {
        case MUTEX_TYPE_PKA:
            sys_mutex_put(&hw_pka_lock);
            break;
        case MUTEX_TYPE_CRYP:
            sys_mutex_put(&hw_cryp_lock);
            break;
        case MUTEX_TYPE_HASH:
            sys_mutex_put(&hw_hash_lock);
            break;
        default:
            break;
    }
}

int ciphersuite_preference_array[MAX_CIPHERSUITE_PREFERENCE_ARRAY_LEN]; //Array dimension is equal to the dimension of ciphersuite_preference_default
extern void mbedtls_ciphersuite_preference_init(int *array);
extern int mbedtls_platform_set_hardware_poll(int (*hardware_poll_func)(void *data, unsigned char *output, size_t len, size_t *olen));
/*!
    \brief      initialize mebdtls platform
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void mebdtls_platform_init(void)
{
#if !defined(CONFIG_TZ_ENABLED)
    /* Reconfig function pointers for MbedTLS */
    mbedtls_platform_set_calloc_free(sys_calloc, sys_mfree);
    mbedtls_platform_set_snprintf(snprintf);
    mbedtls_platform_set_printf(printf);

    mbedtls_platform_set_time((mbedtls_time_t (*)( mbedtls_time_t*))sys_time_get);
    mbedtls_ecp_curve_val_init();
    mbedtls_platform_set_hardware_poll(gd_hardware_poll);
    mbedtls_hwpka_flag_set(0/**/
                        | MBEDTLS_HW_EXP_MOD
                        | MBEDTLS_HW_RSA_PRIVATE
                        | MBEDTLS_HW_ECDSA_SIGN
                        | MBEDTLS_HW_ECDSA_VERIFY
                        | MBEDTLS_HW_ECP_MUL
                        | MBEDTLS_HW_ECP_CHECK
                        | MBEDTLS_HW_MPI_MUL);
#endif
    /* Replace ciphersuite_preference pointer if needed */
    memcpy((void *)ciphersuite_preference_array, ciphersuite_preference_default, MAX_CIPHERSUITE_PREFERENCE_ARRAY_LEN);
    mbedtls_ciphersuite_preference_init(ciphersuite_preference_array);
}

/*!
    \brief      initialize rom
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rom_init(void)
{
    mebdtls_platform_init();

#if defined(CONFIG_TZ_ENABLED)
    ns_interface_init();
#endif

    mbl_mutex_set_func(__LOCK, (void *)sys_mutex_get_func);
    mbl_mutex_set_func(__UNLOCK, (void *)sys_mutex_put_func);
}
