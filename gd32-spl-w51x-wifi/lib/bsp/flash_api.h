/*!
    \file    flash_api.h
    \brief   Flash API for GD32W51x WiFi SDK

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

#ifndef _FLASH_API_H_
#define _FLASH_API_H_

#define FLASH_SIZE_SIP              (0x200000)
#define FLASH_SIP_PAGE_SIZE         (0x1000)

#define FLASH_SIZE_QSPI             (0x2000000 - FLASH_OB_SIZE)
#define FLASH_QSPI_PAGE_SIZE        (0x100)
#define FLASH_QSPI_SECTOR_SIZE      (0x1000)

#define FLASH_TOTAL_SIZE            (is_sip_flash() ? FLASH_SIZE_SIP : FLASH_SIZE_QSPI)
#define IS_SIP_FLASH()              (OBSTAT_NQSPI() == SET)

#define FLASH_OB_SIZE               (0)
#define FLASH_START_QSPI            FLASH_OB_SIZE

/* QSPI FLASH COMMANDs */
#define WRITE_ENABLE_CMD            0x06
#define WRITE_DISABLE_CMD           0x04
#define READ_STATUS_REG1_CMD        0x05
#define WRITE_STATUS_REG1_CMD       0x01
#define READ_STATUS_REG_CMD         0x35
#define WRITE_STATUS_REG_CMD        0x31
#define QUAD_IN_FAST_PROG_CMD       0x32
#define QUAD_OUT_FAST_READ_CMD      0x6B
#define SECTOR_ERASE_CMD            0x20    // 0xD8
#define HIGH_PERFORMANCE_CMD        0xA3
#define CHIP_ERASE_CMD              0xC7
#define CHIP_READ_ID_CMD            0x9F

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
void flash_offset_mapping(uint32_t of_spage, uint32_t of_epage, uint32_t of_value);

/* QSPI FLASH APIs */
void qspi_flash_config(uint32_t clock_prescaler);
int32_t qspi_flash_read_id(void *id);
int32_t qspi_flash_sector_erase(uint32_t address);
int32_t qspi_flash_read(uint32_t address, void *data, uint32_t size);
int32_t qspi_flash_page_program(uint32_t address, const uint8_t *data, uint32_t size);


#endif
