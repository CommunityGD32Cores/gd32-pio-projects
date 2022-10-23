/*!
    \file    rf_efuse.h
    \brief   RF efuse BSP for GD32W51x WiFi SDK

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

#ifndef _RF_EFUSE_H__
#define _RF_EFUSE_H__

typedef int32_t rf_efuse_status_t;

#define RF_EFUSE_SUCCESS                 ((rf_efuse_status_t)0)
#define RF_EFUSE_SET_ERROR               ((rf_efuse_status_t)-1)
#define RF_EFUSE_GET_ERROR               ((rf_efuse_status_t)-2)
#define RF_EFUSE_CHECK_ERROR             ((rf_efuse_status_t)-3)
#define RF_EFUSE_OFFSET_ERROR            ((rf_efuse_status_t)-4)
#define RF_EFUSE_BAD_INPUT_ERROR         ((rf_efuse_status_t)-5)

#define MAX_RF_EFUSE_ITEM        8
#define RF_EFUSE_ITEM_MAX_LEN    4


rf_efuse_status_t rf_efuse_write(uint8_t item, uint8_t size, uint8_t *pdata);
rf_efuse_status_t rf_efuse_read(uint8_t item, uint8_t size, uint8_t *pdata);
rf_efuse_status_t rf_efuse_item_consumed_bytes_get(uint8_t item, uint8_t size, uint8_t *pdata, uint8_t *result);
uint8_t rf_efuse_available_space_get(void);
uint8_t rf_efuse_totale_space_get(void);
uint8_t *rf_efuse_map_get(void);
rf_efuse_status_t rf_efuse_map_reload(void);
uint32_t rf_efuse_reg_read(void);
uint32_t rf_efuse_type_reg_read(void);

#endif
