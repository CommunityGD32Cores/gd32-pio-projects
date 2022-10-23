/*!
    \file    mbl_flash.h
    \brief   Non-secure MBL flash file for GD32W51x WiFi SDK

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

#ifndef __MBL_FLASH_H__
#define __MBL_FLASH_H__

#include "mbl_qspi_flash.h"

#define FLASH_SIZE_SIP              (0x200000)
#define FLASH_SIP_PAGE_SIZE         (0x1000)

#define FLASH_TOTAL_SIZE            (is_sip_flash() ? FLASH_SIZE_SIP : FLASH_SIZE_QSPI)
#define IS_SIP_FLASH()              (OBSTAT_NQSPI() == SET)

int is_flash_cache_enabled(void);
int flash_cache_disable(void);
int flash_cache_enable(void);

int is_sip_flash(void);
int is_valid_flash_offset(uint32_t offset);
int is_valid_flash_addr(uint32_t addr);
uint32_t flash_total_size(void);
uint32_t flash_erase_size(void);
int flash_init(void);
int flash_indirect_read(uint32_t offset, void *data, int len);
int flash_read(uint32_t offset, void *data, int len);
int flash_write(uint32_t offset, const void *data, int len);
int flash_fast_write(uint32_t offset, const void *data, int len);
int flash_erase(uint32_t offset, int len);
void flash_nodec_config(uint32_t nd_idx, uint32_t start_page, uint32_t end_page);
void flash_offset_mapping(void);

#endif  /* __MBL_FLASH_H__ */
