/*!
    \file    rf_efuse.c
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

#include "platform_def.h"
#include "stdint.h"
#include "string.h"
#include "wrapper_os.h"
#include "rf_efuse.h"
#include "bsp_inc.h"
#if 0  // defined(CONFIG_TZ_ENABLED)
#include "mbl_nsc_api.h"
#define efuse_rf_set efuse_rf_set_nsc
#define efuse_rf_get efuse_rf_get_nsc
#else
#endif

#define EFUSE_NULL                0x00


/** RF EFUSE Header layout
+++++++++++++++++++++++++++
|    7:4    |     3:0     |
===========================
| Item Type | Byte Bit Map|
+++++++++++++++++++++++++++
**/

#define RF_EFUSE_HDR_SIZE          0x01

// Item type [7:4]
#define RF_EFUSE_ITEM_MASK         0xf0
#define RF_EFUSE_ITEM_POS          0x04

// Byte Bit Map [3:0]
#define RF_EFUSE_BIT_MAP_MASK      0x0f
#define RF_EFUSE_BIT_MAP_POS       0x00

#define RF_EFUSE_ITEM_MAX_BYTES    0x04

#define EFUSE_SIZE_RF              (EFUSE_WD_RF - 1)

uint8_t rf_efuse_map[MAX_RF_EFUSE_ITEM * RF_EFUSE_ITEM_MAX_LEN];

#if 1  // !defined(CONFIG_TZ_ENABLED)

/*!
    \brief      set rf efuse
    \param[in]  offset: EFUSE offset address
    \param[in]  sz: size of EFUSE
      \arg        1~0x40
    \param[in] rf: the buffer for storing write-into EFUSE register value
    \retval     error status: ERROR or SUCCESS
*/
int efuse_rf_set(uint8_t *rf, uint32_t offset, uint32_t sz)
{
    int ret;
    ret = efuse_write(EFUSE_TYPE_RF, offset, sz, rf);
    if (ret == ERROR)
        ret = -1;

    return ret;
}

/*!
    \brief      get rf efuse
    \param[in]  offset: EFUSE offset address
    \param[in]  sz: size of EFUSE
      \arg        1~0x40
    \param[out] rf: the buffer for storing read-out EFUSE register value
    \retval     error status: ERROR or SUCCESS
*/
int efuse_rf_get(uint8_t *rf, uint8_t offset, uint8_t sz)
{
    int ret;
    sys_memset(rf, 0, sz);
    ret = efuse_read(EFUSE_TYPE_RF, offset, sz, rf);
    if (ret == ERROR)
        return -1;

    return ret;
}
#endif

/*!
    \brief       read rf efuse data to map
    \param[in]   none
    \param[out]  pbuf : MAX_RF_EFUSE_ITEM * RF_EFUSE_ITEM_MAX_LEN buffer for the rf map
    \retval      rf_efuse_status_t: read status
*/
rf_efuse_status_t rf_efuse_map_read(uint8_t *pbuf)
{
    uint8_t rf_efuse[EFUSE_SIZE_RF];
    uint8_t rf_offset;
    uint8_t efuse_data;
    uint8_t addr;
    uint8_t map;
    uint8_t i;

    rf_offset = 0;

    // default 0x00
    sys_memset(pbuf, EFUSE_NULL, MAX_RF_EFUSE_ITEM * RF_EFUSE_ITEM_MAX_LEN);

    if (efuse_rf_get(rf_efuse, 0, EFUSE_SIZE_RF) < 0) {
        return RF_EFUSE_GET_ERROR;
    }

    while (rf_offset < EFUSE_SIZE_RF) {
        efuse_data = rf_efuse[rf_offset++];

        if (efuse_data == EFUSE_NULL)
            break;

        addr = ((efuse_data & RF_EFUSE_ITEM_MASK) >> RF_EFUSE_ITEM_POS) * RF_EFUSE_ITEM_MAX_LEN;
        map = (efuse_data & RF_EFUSE_BIT_MAP_MASK) >> RF_EFUSE_BIT_MAP_POS;

        for (i = 0; i < RF_EFUSE_ITEM_MAX_BYTES; i++) {
            if (map & (0x01 << i)) {
                pbuf[addr] = rf_efuse[rf_offset++];
            }
            addr++;
        }
    }

    return RF_EFUSE_SUCCESS;

}

/*!
    \brief       load rf map
    \param[in]   none
    \param[out]  pbuf : MAX_RF_EFUSE_ITEM * RF_EFUSE_ITEM_MAX_LEN buffer for the rf map
    \retval      rf_efuse_status_t: read status
*/
rf_efuse_status_t rf_efuse_map_load()
{
    static uint8_t loaded = 0;
    rf_efuse_status_t ret;

    if (!loaded) {
        if ((ret = rf_efuse_map_read(rf_efuse_map)) == RF_EFUSE_SUCCESS) {
            loaded = 1;
        } else {
            return ret;
        }
    }

    return RF_EFUSE_SUCCESS;
}

/*!
    \brief       get physical efuse unused offset
    \param[in]   none
    \param[out]  offset: the result of unused physical efuse map offset
    \retval      rf_efuse_status_t: read status
*/
rf_efuse_status_t rf_efuse_unused_offset_get(uint8_t *offset)
{
    uint8_t efuse_data;
    uint8_t rf_offset = 0;
    uint8_t map;
    uint8_t i;

    while (rf_offset < EFUSE_SIZE_RF) {
        if (efuse_rf_get(&efuse_data, rf_offset, 1) < 0) {
            return RF_EFUSE_GET_ERROR;
        }

        if (efuse_data == EFUSE_NULL)
            break;

        rf_offset++;

        map = (efuse_data & RF_EFUSE_BIT_MAP_MASK) >> RF_EFUSE_BIT_MAP_POS;

        for (i = 0; i < RF_EFUSE_ITEM_MAX_BYTES; i++) {
            if (map & (0x01 << i)) {
                rf_offset++;
            }
        }
    }

    if (rf_offset >= EFUSE_SIZE_RF)
        return RF_EFUSE_OFFSET_ERROR;

    *offset = rf_offset;

    return RF_EFUSE_SUCCESS;
}

/*!
    \brief       write one byte to rf physical efuse and check
    \param[in]   offset: the offset of rf physical efuse to write
    \param[in]   data: the data to write
    \param[out]  none
    \retval      rf_efuse_status_t: read status
*/
rf_efuse_status_t rf_efuse_one_byte_write_check(uint8_t offset, uint8_t data)
{
    uint8_t write_data;
    uint8_t read_data;

    write_data = data;
    if (efuse_rf_set(&write_data, offset, 1) < 0) {
        return RF_EFUSE_SET_ERROR;
    }

    if (efuse_rf_get(&read_data, offset, 1) < 0) {
        return RF_EFUSE_GET_ERROR;
    }

    if (write_data != read_data)
        return RF_EFUSE_CHECK_ERROR;

    return RF_EFUSE_SUCCESS;
}

/*!
    \brief       write rf item to rf physical efuse and update rf map
    \param[in]   item: the item type
    \param[in]   size: the size of buffer
    \param[in]   pdata: the buffer to write
    \param[out]  none
    \retval      rf_efuse_status_t: read status
*/
rf_efuse_status_t rf_efuse_write(uint8_t item, uint8_t size, uint8_t *pdata)
{
    uint8_t new_data[RF_EFUSE_ITEM_MAX_LEN];
    uint8_t addr;
    uint8_t hdr;
    uint8_t rf_offset;
    uint8_t i;
    uint8_t bit_map;
    uint8_t new_size;
    rf_efuse_status_t ret;

    if (pdata == NULL || size == 0 || item > MAX_RF_EFUSE_ITEM) {
        ret = RF_EFUSE_BAD_INPUT_ERROR;
        goto FAIL_OUT;
    }

    if((ret = rf_efuse_map_load()) != RF_EFUSE_SUCCESS)
        goto FAIL_OUT;

    if ((ret = rf_efuse_unused_offset_get(&rf_offset)) != RF_EFUSE_SUCCESS)
        goto FAIL_OUT;

    addr = item * RF_EFUSE_ITEM_MAX_LEN;

    if (size <= RF_EFUSE_ITEM_MAX_LEN) {
        new_size = 0x00;
        bit_map = 0x00;

        // find out difference
        for (i = 0; i < size; i ++) {
            if (rf_efuse_map[addr+i] != pdata[i]) {
                new_data[new_size++] = pdata[i];
                bit_map |= (1 << i);
            }
        }

        // something changed
        if (bit_map) {
            if ((new_size + rf_offset + RF_EFUSE_HDR_SIZE) > EFUSE_SIZE_RF) {
                ret = RF_EFUSE_OFFSET_ERROR;
                goto FAIL_OUT;
            }

            hdr = ((item << RF_EFUSE_ITEM_POS) & RF_EFUSE_ITEM_MASK) |
                    ((bit_map << RF_EFUSE_BIT_MAP_POS) & RF_EFUSE_BIT_MAP_MASK);
            if ((ret = rf_efuse_one_byte_write_check(rf_offset++, hdr)) != RF_EFUSE_SUCCESS)
                goto FAIL_OUT;

            for (i = 0; i < new_size; i++) {
                if ((ret = rf_efuse_one_byte_write_check(rf_offset + i, new_data[i])) != RF_EFUSE_SUCCESS)
                    goto FAIL_OUT;
            }
        }

    } else {
        ret = RF_EFUSE_BAD_INPUT_ERROR;
        goto FAIL_OUT;
    }

    // update map
    memcpy(&rf_efuse_map[addr], pdata, size);

    return RF_EFUSE_SUCCESS;

FAIL_OUT:
    return ret;
}

/*!
    \brief       read rf map item to buffer
    \param[in]   item: the item type
    \param[in]   size: the size of buffer
    \param[in]   pdata: the buffer to read out
    \param[out]  none
    \retval      rf_efuse_status_t: read status
*/
rf_efuse_status_t rf_efuse_read(uint8_t item, uint8_t size, uint8_t *pdata)
{
    uint8_t addr;
    rf_efuse_status_t ret;
    if (pdata == NULL || size == 0 || size > RF_EFUSE_ITEM_MAX_LEN || item > MAX_RF_EFUSE_ITEM) {
        ret = RF_EFUSE_BAD_INPUT_ERROR;
        goto FAIL_OUT;
    }

    if((ret = rf_efuse_map_load()) != RF_EFUSE_SUCCESS)
        goto FAIL_OUT;

    addr = item * RF_EFUSE_ITEM_MAX_LEN;
    memcpy(pdata, &rf_efuse_map[addr], size);

    return RF_EFUSE_SUCCESS;

FAIL_OUT:
    return ret;
}

/*!
    \brief       get the consumed efuse bytes of updating item
    \param[in]   item: the item type
    \param[in]   size: the size of buffer
    \param[in]   pdata: the buffer to write
    \param[out]  result: the consumed bytes of updating item
    \retval      rf_efuse_status_t: read status
*/
rf_efuse_status_t rf_efuse_item_consumed_bytes_get(uint8_t item, uint8_t size, uint8_t *pdata, uint8_t *result)
{
    uint8_t addr;
    uint8_t i;
    uint8_t new_size;
    rf_efuse_status_t ret;

    if (pdata == NULL || size == 0 || item > MAX_RF_EFUSE_ITEM) {
        ret = RF_EFUSE_BAD_INPUT_ERROR;
        goto FAIL_OUT;
    }

    if((ret = rf_efuse_map_load()) != RF_EFUSE_SUCCESS)
        goto FAIL_OUT;

    addr = item * RF_EFUSE_ITEM_MAX_LEN;

    if (size <= RF_EFUSE_ITEM_MAX_LEN) {
        new_size = 0x00;
        for (i = 0; i < size; i ++) {
            if (rf_efuse_map[addr+i] != pdata[i]) {
                new_size++;
            }
        }

        if (new_size) {
            *result = new_size + RF_EFUSE_HDR_SIZE;
        } else {
            *result = 0;
        }
    } else {
        ret = RF_EFUSE_BAD_INPUT_ERROR;
        goto FAIL_OUT;
    }

    return RF_EFUSE_SUCCESS;

FAIL_OUT:
    return ret;
}

/*!
    \brief       get available space of rf physical efuse
    \param[in]   none
    \param[out]  none
    \retval      the size of available space(0x00-0xff)
*/
uint8_t rf_efuse_available_space_get(void)
{
    uint8_t rf_offset;

    if ((rf_efuse_unused_offset_get(&rf_offset)) != RF_EFUSE_SUCCESS)
        return 0;
    else
        return EFUSE_SIZE_RF - rf_offset;
}

/*!
    \brief       get total space of rf  physical efuse
    \param[in]   none
    \param[out]  none
    \retval      the size of total space(0x00-0xff)
*/
uint8_t rf_efuse_totale_space_get(void)
{
    return EFUSE_SIZE_RF;
}

/*!
    \brief       get the pointer of rf efuse map
    \param[in]   none
    \param[out]  none
    \retval      the pointer of rf efuse map(0x00000000-0xffffffff)
*/
uint8_t *rf_efuse_map_get(void)
{
    return rf_efuse_map;
}

/*!
    \brief       reload rf efuse data to map
    \param[in]   none
    \param[out]  none
    \retval      rf_efuse_status_t: read status
*/
rf_efuse_status_t rf_efuse_map_reload(void)
{
    return rf_efuse_map_read(rf_efuse_map);
}

/*!
    \brief       read rf efuse register
    \param[in]   none
    \param[out]  none
    \retval      read rf efuse register value(0x00000000-0xffffffff)
*/
uint32_t rf_efuse_reg_read(void)
{
    return *((volatile uint32_t *)RF_THERMAL_REG);
}

uint32_t rf_efuse_type_reg_read(void)
{
    return *((volatile uint32_t *)TYPE_REG);
}