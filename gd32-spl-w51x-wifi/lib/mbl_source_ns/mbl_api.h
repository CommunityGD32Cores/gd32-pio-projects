/*!
    \file    mbl_api.h
    \brief   Non-secure MBL API for GD32W51x WiFi SDK

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

#ifndef __MBL_API_H__
#define __MBL_API_H__
#include "mbl_region.h"
#include "mbl_trace.h"
#include "mbl_sys.h"
#include "mbl_image.h"
#include "mbl_flash.h"
#include "rom_api.h"

#define MBL_MAX_API_NUM     (MBL_API_SIZE >> 2)

struct mbl_api_t {
    void (*sys_status_show)(void);
    void (*sys_status_dump)(int is_ping);
    int (*sys_status_get)(uint8_t type, uint8_t len, uint8_t* pval);
    int (*sys_status_set)(uint8_t type, uint8_t len, uint8_t* pval);
    int (*sys_err_process_set)(uint8_t method);
    int (*sys_trace_level_set)(uint8_t trace_level);
    int (*sys_img_flag_set)(uint8_t idx, uint8_t mask, uint8_t flag);
    int (*sys_running_img_set)(uint8_t idx);
    int (*sys_fw_version_set)(uint32_t type, uint32_t version);
    int (*sys_trng_seed_set)(uint8_t val);
    int (*flash_indirect_read)(uint32_t offset, void *data, int len);
    int (*flash_fast_write)(uint32_t offset, const void *data, int len);
    int (*flash_erase)(uint32_t offset, int len);
};

#if (RE_IMG_0_NSPE_OFFSET == 0)
/* 0x08000000: NSPE  */
#define mbl_sys_status_show()               mbl_dummy_func(__func__, "mbl_sys_status_show")
#define mbl_sys_status_dump(a)              mbl_dummy_func(__func__, "mbl_sys_status_dump")
#define mbl_sys_status_get(a, b, c)         mbl_dummy_func(__func__, "mbl_sys_status_get")
#define mbl_sys_status_set(a, b, c)         mbl_dummy_func(__func__, "mbl_sys_status_set")
#define mbl_sys_set_err_process(a)          mbl_dummy_func(__func__, "mbl_sys_set_err_process")
#define mbl_sys_trace_level_set(a)          mbl_dummy_func(__func__, "mbl_sys_trace_level_set")
#define mbl_sys_img_flag_set(a, b, c)       mbl_dummy_func(__func__, "mbl_sys_img_flag_set")
#define mbl_sys_running_img_set(a)          mbl_dummy_func(__func__, "mbl_sys_running_img_set")
#define mbl_sys_fw_version_set(a, b)        mbl_dummy_func(__func__, "mbl_sys_fw_version_set")
#define mbl_sys_trng_seed_set(a)            mbl_dummy_func(__func__, "mbl_sys_trng_seed_set")
#define mbl_flash_indirect_read(a, b, c)    mbl_dummy_func(__func__, "mbl_flash_indirect_read")
#define mbl_flash_write(a, b, c)            mbl_dummy_func(__func__, "mbl_flash_write")
#define mbl_flash_erase(a, b)               mbl_dummy_func(__func__, "mbl_flash_erase")
static __INLINE int mbl_dummy_func(const char *caller, const char *func)
{
    printf("%s: %s is dummy.\r\n", caller, func);
    return -1;
}
#else
/* 0x08000000: MBL  */
extern struct mbl_api_t *p_mbl_api;
#define mbl_sys_status_show                 p_mbl_api->sys_status_show
#define mbl_sys_status_dump                 p_mbl_api->sys_status_dump
#define mbl_sys_status_get                  p_mbl_api->sys_status_get
#define mbl_sys_status_set                  p_mbl_api->sys_status_set
#define mbl_sys_set_err_process             p_mbl_api->sys_err_process_set
#define mbl_sys_trace_level_set             p_mbl_api->sys_trace_level_set
#define mbl_sys_img_flag_set                p_mbl_api->sys_img_flag_set
#define mbl_sys_running_img_set             p_mbl_api->sys_running_img_set
#define mbl_sys_fw_version_set              p_mbl_api->sys_fw_version_set
#define mbl_sys_trng_seed_set               p_mbl_api->sys_trng_seed_set
#define mbl_flash_indirect_read             p_mbl_api->flash_indirect_read
#define mbl_flash_write                     p_mbl_api->flash_fast_write
#define mbl_flash_erase                     p_mbl_api->flash_erase
#endif  /*(RE_IMG_0_NSPE_OFFSET == 0) */

extern struct rom_api_s_t *p_rom_api_s;
#define mbl_mutex_set_func                    p_rom_api_s->set_mutex_func

#endif  //__MBL_API_H__
