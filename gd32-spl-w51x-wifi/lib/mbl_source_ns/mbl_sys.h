/*!
    \file    mbl_sys.h
    \brief   Non-secure MBL system file for GD32W51x WiFi SDK

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

#ifndef __MBL_SYS_H__
#define __MBL_SYS_H__

#include "mbl_region.h"

/* ============================================================= */
#define FLASH_OFFSET_SYS_STATUS_PING     (RE_SYS_STATUS_OFFSET)
#define FLASH_OFFSET_SYS_STATUS_PONG     (FLASH_OFFSET_SYS_STATUS_PING + SYS_STATUS_AREA_SZ)
#define FLASH_OFFSET_IMG_VERSION_LOCK    (FLASH_OFFSET_SYS_STATUS_PING - 4)
#define SYS_STATUS_SALT                  "sys_gigadevice"

/* System status related size */
#define SYS_STATUS_AREA_SZ            0x1000
#define SYS_STATUS_HEADER_SZ          (sizeof(struct sys_status_header_t))
#define SYS_STATUS_TLV_HEADER_SZ      2
#define MAX_TLV_VALUE_SIZE            16
#define SYS_STATUS_MAGIC_CODE         0x19342750
#define AES_KEY_SZ                    16
#define AES_BLOCK_SZ                  AES_KEY_SZ

/* System status error definitions */
#define SYS_STATUS_OK                0
#define SYS_STATUS_ERR_FLASH         1
#define SYS_STATUS_ERR_KEY           (1 << 1)
#define SYS_STATUS_ERR_TOTAL_LEN     (1 << 2)
#define SYS_STATUS_ERR_MAGIC         (1 << 3)
#define SYS_STATUS_ERR_CHECKSUM      (1 << 4)
#define SYS_STATUS_ERR_TLV           (1 << 5)
#define SYS_STATUS_ERR_CRYPT         (1 << 6)
#define SYS_STATUS_ERR_MEM           (1 << 7)
#define SYS_STATUS_ERR_INPUT         (1 << 8)

/* System status item length */
#define LEN_SYS_ERROR_PROCESS        1
#define LEN_SYS_TRACE_LEVEL          1
#define LEN_SYS_IMAGE_STATUS         1
#define LEN_SYS_RUNNING_IMG          1
#define LEN_SYS_VER_COUNTER          4
#define LEN_SYS_TRNG_SEED            1

#define LEN_SYS_AUTO_CONN            1
#define LEN_SYS_SSID_X               16
#define LEN_SYS_PASSWD_X             16
#define LEN_SYS_IP_ADDR              4

/* ERROR process  */
#define ERR_PROCESS_REDOWNLOD        0
#define ERR_PROCESS_ENDLESS_LOOP     1

/* SYS status found result */
#define SYS_STATUS_FOUND_OK          0
#define SYS_STATUS_NOT_FOUND         -1
#define SYS_STATUS_FOUND_ERR         -2

/* There are two flash blocks used to manage system status: Ping, Pong.
    The format of system status is shown as below.
    | total size   |  valid count  | Magic   | Checksum |
    | type | len | value|
    valid count - used to indicate which is active, Ping or Pong. A higher valid count represents the active system status.
    total size - the total size of the following TLVs
 */
enum {
    SYS_ERROR_PROCESS = 1,          /* 0: Redownload; 1: while(1) */
    SYS_TRACE_LEVEL = 2,            /* The Trace Level for IBL and MBL */
    SYS_IMAGE0_STATUS = 3,          /* the status of Main Image 0, enum image_status */
    SYS_IMAGE1_STATUS = 4,          /* the status of Main Image 1, enum image_status */
    SYS_RUNNING_IMG = 5,            /* 0: Image0, 1: Image1 */
    SYS_MBL_VER_COUNTER = 6,        /* NV counter for MBL version */
    SYS_IMG_VER_COUNTER = 7,        /* NV counter for Image version */
    SYS_MBLPK_VER_COUNTER = 8,      /* NV counter for MBL Public Key version */
    SYS_AROTPK_VER_COUNTER = 9,     /* NV counter for AROT Public Key version */
    SYS_TRNG_SEED = 10,             /* 1: Rand use TRNG seed, 0: software seed */

    /* Defined by Application */
    SYS_AUTO_CONN,
    SYS_SSID_1,
    SYS_SSID_2,
    SYS_PASSWD_1,
    SYS_PASSWD_2,
    SYS_PASSWD_3,
    SYS_PASSWD_4,
    SYS_IP_ADDR,
    SYS_UNKNOWN_TYPE = 0xFF
};

struct sys_status_header_t {
    uint32_t tot_len;
    uint32_t act_cntr;
    uint32_t magic;
    uint32_t checksum;
};

extern int32_t mbl_err_process;
extern int32_t mbl_trace_level;

/* ============================================================= */
int sys_status_init(void);
int sys_statu_integritys_check(void);
int sys_status_set(uint8_t type, uint8_t len, uint8_t* pval);
int sys_status_get(uint8_t type, uint8_t len, uint8_t* pval);
int sys_err_process_set(uint8_t method);
int sys_trace_level_set(uint8_t trace_level);
int sys_img_flag_set(uint8_t idx, uint8_t mask, uint8_t flag);
int sys_running_img_set(uint8_t idx);
int sys_fw_version_set(uint32_t type, uint32_t version);
int sys_trng_seed_set(uint8_t val);
void sys_status_show(void);
void sys_status_dump(int is_ping);
int sys_status_check(void);

#endif //__MBL_SYS_H__
