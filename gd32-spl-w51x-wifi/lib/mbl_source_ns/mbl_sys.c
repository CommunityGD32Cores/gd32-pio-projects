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

#include "gd32w51x.h"
#include "mbl_trace.h"
#include "mbl_image.h"
#include "mbl_sys.h"
#include "mbl_flash.h"
#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "mbedtls/md5.h"
#include "mbedtls/memory_buffer_alloc.h"

const uint8_t valid_image_status[] = {
    (IMG_FLAG_OLDER | IMG_FLAG_VERIFY_NONE | IMG_FLAG_IA_NONE),  // b'00000, Reset
    (IMG_FLAG_NEWER | IMG_FLAG_VERIFY_NONE | IMG_FLAG_IA_NONE),  // b'00001, New image downloaded
    (IMG_FLAG_NEWER | IMG_FLAG_VERIFY_OK | IMG_FLAG_IA_NONE),    // b'00011, Newer image and MBL verify it OK
    (IMG_FLAG_OLDER | IMG_FLAG_VERIFY_OK | IMG_FLAG_IA_NONE),    // b'00010, Older image and MBL verify it OK
    (IMG_FLAG_NEWER | IMG_FLAG_VERIFY_FAIL | IMG_FLAG_IA_NONE),  // b'00101, MBL verify it failed
    (IMG_FLAG_NEWER | IMG_FLAG_VERIFY_FAIL | IMG_FLAG_IA_OK),    // b'01101, Newer, IA OK, but MBL verify it failed after next reboot
    (IMG_FLAG_NEWER | IMG_FLAG_VERIFY_OK | IMG_FLAG_IA_OK),      // b'01011, Newer image and MBL verify OK and Initial Attestation OK
    (IMG_FLAG_OLDER | IMG_FLAG_VERIFY_OK | IMG_FLAG_IA_OK),      // b'01010, Older image and MBL verify OK and Initial Attestation OK
    (IMG_FLAG_NEWER | IMG_FLAG_VERIFY_OK | IMG_FLAG_IA_FAIL),    // b'10011, Initial Attestation failed
};

/*!
    \brief      get image valid status
    \param[in]  adress: flash's internal address to write to
    \param[out] none
    \retval     status of image(1: valid, or 0: invalid)
*/
int image_valid_status_get(uint8_t status)
{
    uint32_t sz = sizeof(valid_image_status);
    int i;
    int ret = 0;
    for (i = 0; i < sz; i++) {
        if (status == valid_image_status[i]) {
            ret = 1;
            break;
        }
    }
    return ret;
}

/*!
    \brief      print system status such as type, length and value
    \param[in]  t: type of system status
    \param[in]  l: length of system status
    \param[in]  v: pointer to value of system status
    \param[out] none
    \retval     none
*/
static void tlv_print(uint8_t t, uint8_t l, uint8_t *v)
{
    int i;
    printf("%d\t%d\t", t, l);
    for (i = 0; i < l; i++) {
        if (i == l -1)
            printf("%02x", v[i]);
        else
            printf("%02x-", v[i]);
    }
    printf("\r\n");
}

/*!
    \brief      initialize mbedtls
    \param[in]  none
    \param[out] none
    \retval     none
*/
void mbedtls_init(void)
{
    static uint8_t alloc_buf[0x1000];
    mbedtls_memory_buffer_alloc_init(alloc_buf, sizeof(alloc_buf));
    rcu_periph_clock_enable(RCU_HAU);
    rcu_periph_clock_enable(RCU_CAU);
    rcu_periph_clock_enable(RCU_PKCAU);
}

/*!
    \brief      encrypt or decrypt operation for data
    \param[in]  mode: encryption or decryption operation
      \arg        MBEDTLS_AES_ENCRYPT: encryption operation
      \arg        MBEDTLS_AES_DECRYPT: decryption operation
    \param[in]  input: pointer to the buffer store input data
    \param[in]  length: length of input data
    \param[out] output: pointer to the buffer store output data
    \retval     encrypt or decrypt result(0: ok, or other value: input length on error)
*/
static int sys_status_crypt(IN uint8_t mode,
                            IN uint8_t *input,
                            IN size_t length,
                            OUT uint8_t *output)
{
    mbedtls_aes_context ctx;
    uint8_t key[AES_KEY_SZ], iv[AES_KEY_SZ];
    int keybits = AES_KEY_SZ * 8;
    int ret = 0;

    if ((length % AES_BLOCK_SZ != 0) || length == 0)
        return -1;

    memset(key, 0, sizeof(key));
    memcpy(key, SYS_STATUS_SALT, strlen(SYS_STATUS_SALT));
    mbedtls_md5_ret(key, AES_KEY_SZ, key);

    mbedtls_aes_init(&ctx);

    if (mode == MBEDTLS_AES_ENCRYPT) {
        mbedtls_aes_setkey_enc(&ctx, key, keybits);
    } else {
        mbedtls_aes_setkey_dec(&ctx, key, keybits);
    }

    ret = mbedtls_aes_crypt_ecb(&ctx, mode, input, output);
    if (ret != 0) goto exit;
    input += AES_BLOCK_SZ;
    length -= AES_BLOCK_SZ;

    if (length > 0) {
        memset(iv , 0x5A, AES_KEY_SZ);
        output += AES_BLOCK_SZ;
        ret = mbedtls_aes_crypt_cbc(&ctx, mode, length, iv, input, output);
    }

exit:
    memset(key, 0, sizeof(key));
    mbedtls_aes_free(&ctx);
    return ret;
}

/*!
    \brief      get active header
    \param[in]  none
    \param[out] sys_hdr: pointer to the header of system status(ping or pong)
    \param[out] flash_offset: pointer to the flash offset of ping or pong

    \retval     status of get active header
      \arg        SYS_STATUS_OK: status is ok
      \arg        SYS_STATUS_ERR_FLASH: flash error
      \arg        SYS_STATUS_ERR_CRYPT: crypt error
      \arg        SYS_STATUS_ERR_MAGIC: magic error
*/
static int active_header_get(OUT struct sys_status_header_t *sys_hdr,
                             OUT uint32_t *flash_offset)
{
    struct sys_status_header_t *hdr;
    uint8_t blk_ping[AES_BLOCK_SZ], blk_pong[AES_BLOCK_SZ];
    uint32_t cnt_ping, cnt_pong;
    int ping_valid = 1, pong_valid = 1;
    int is_ping, ret, status;

    /* Read PING header */
    ret = flash_indirect_read(FLASH_OFFSET_SYS_STATUS_PING, blk_ping, AES_BLOCK_SZ);
    if (ret != 0) {
        status = SYS_STATUS_ERR_FLASH;
        goto exit;
    }
    ret = sys_status_crypt(MBEDTLS_AES_DECRYPT, blk_ping, AES_BLOCK_SZ, blk_ping);
    if (ret != 0) {
        status = SYS_STATUS_ERR_CRYPT;
        goto exit;
    }
    hdr = (struct sys_status_header_t *)blk_ping;
    cnt_ping = hdr->act_cntr;
    if (hdr->magic != SYS_STATUS_MAGIC_CODE) {
        ping_valid = 0;
    }

    /* Read PONG header */
    ret = flash_indirect_read(FLASH_OFFSET_SYS_STATUS_PONG, blk_pong, AES_BLOCK_SZ);
    if (ret != 0) {
        status = SYS_STATUS_ERR_FLASH;
        goto exit;
    }
    ret = sys_status_crypt(MBEDTLS_AES_DECRYPT, blk_pong, AES_BLOCK_SZ, blk_pong);
    if (ret != 0) {
        status = SYS_STATUS_ERR_CRYPT;
        goto exit;
    }
    hdr = (struct sys_status_header_t *)blk_pong;
    cnt_pong = hdr->act_cntr;
    if (hdr->magic != SYS_STATUS_MAGIC_CODE) {
        pong_valid = 0;
    }

    if ((ping_valid == 0) && ((pong_valid == 0))) {
        status = SYS_STATUS_ERR_MAGIC;
        goto exit;
    } else if ((ping_valid == 0) && ((pong_valid == 1))) {
        is_ping = 0;
    } else if ((ping_valid == 1) && ((pong_valid == 0))) {
        is_ping = 1;
    } else {
        /* Which is active, PING or PONG ? */
        if (cnt_ping == 0xFFFFFFFF)  /* First use, valid count is from 0 to 0xFFFFFFFE */
            cnt_ping = 0;
        if (cnt_pong == 0xFFFFFFFF)  /* First use, valid count is from 0 to 0xFFFFFFFE */
            cnt_pong = 0;

        if ((cnt_ping == (cnt_pong + 1)) || (cnt_ping == (cnt_pong + 2))) {
            is_ping = 1;
        } else {
            is_ping = 0;
        }
    }

    if (is_ping) {
        memcpy((void*)sys_hdr, blk_ping, SYS_STATUS_HEADER_SZ);
        *flash_offset = FLASH_OFFSET_SYS_STATUS_PING;
    } else {
        memcpy((void*)sys_hdr, blk_pong, SYS_STATUS_HEADER_SZ);
        *flash_offset = FLASH_OFFSET_SYS_STATUS_PONG;
    }

    status = SYS_STATUS_OK;
exit:
    return status;
}

/*!
    \brief      find the address offset of type, length, and value according to type
    \param[in]  buf: pointer to the the buffer store type, length and value
    \param[in]  len: length of type, length and value
    \param[in]  type: type of system status
    \param[out] none
    \retval     status of find(-1: find error)
*/
static int tlv_find_by_type(uint8_t *buf, uint32_t len, uint8_t type)
{
    uint8_t *p, t, l;
    uint32_t offset = 0;
    int find = -1;

    /* Check if the type has already existed. */
    p = buf;
    while (offset < len) {
        t = *p;  l = *(p + 1);
        if (t == type) {
            find = offset;
            break;
        }
        p += SYS_STATUS_TLV_HEADER_SZ + l;
        offset += SYS_STATUS_TLV_HEADER_SZ + l;
    }
    return find;
}

/*!
    \brief      initialize system status
    \param[in]  none
    \param[out] none
    \retval     status of initialize
      \arg        SYS_STATUS_OK: status is ok
      \arg        SYS_STATUS_ERR_CRYPT: crypt error
      \arg        SYS_STATUS_ERR_FLASH: flash error
*/
int sys_status_init(void)
{
    struct sys_status_header_t sys_hdr;
    int ret, status = SYS_STATUS_OK;

    /* Fill with an empty header of sys status */
    sys_hdr.tot_len = AES_BLOCK_SZ;
    sys_hdr.act_cntr = 0;
    sys_hdr.magic = SYS_STATUS_MAGIC_CODE;
    sys_hdr.checksum = (sys_hdr.tot_len ^ sys_hdr.magic);
    ret = sys_status_crypt(MBEDTLS_AES_ENCRYPT, (uint8_t*)&sys_hdr, AES_BLOCK_SZ, (uint8_t*)&sys_hdr);
    if (ret != 0) {
        status = SYS_STATUS_ERR_CRYPT;
        goto exit;
    }

    /* Erase PING and PONG */
    ret = flash_erase(FLASH_OFFSET_SYS_STATUS_PING, SYS_STATUS_AREA_SZ * 2);
    if (ret != 0) {
        status = SYS_STATUS_ERR_FLASH;
        goto exit;
    }

    ret = flash_write(FLASH_OFFSET_SYS_STATUS_PING, (void*)&sys_hdr, AES_BLOCK_SZ);
    if (ret != 0) {
        status = SYS_STATUS_ERR_FLASH;
        goto exit;
    }
    ret = flash_write(FLASH_OFFSET_SYS_STATUS_PONG, (void*)&sys_hdr, AES_BLOCK_SZ);
    if (ret != 0) {
        status = SYS_STATUS_ERR_FLASH;
        goto exit;
    }
exit:
    return status;
}

/*!
    \brief      check integrity of system status
    \param[in]  none
    \param[out] none
    \retval     status of check
      \arg        SYS_STATUS_OK: status is ok
      \arg        SYS_STATUS_ERR_TOTAL_LEN: total length error
      \arg        SYS_STATUS_ERR_MAGIC: magic error
      \arg        SYS_STATUS_ERR_MEM: memory error
      \arg        SYS_STATUS_ERR_FLASH: flash error
      \arg        SYS_STATUS_ERR_CRYPT: crypt error
      \arg        SYS_STATUS_ERR_CHECKSUM: checksum error
      \arg        SYS_STATUS_ERR_TLV: tlv error
*/
int sys_statu_integritys_check(void)
{
    struct sys_status_header_t sys_hdr;
    uint32_t flash_offset, alen, sz;
    uint8_t *buf = NULL, *p, t, l;
    int ret, status;

    /* Found the active system status */
    status = active_header_get(&sys_hdr, &flash_offset);
    if (status != SYS_STATUS_OK) {
        goto exit;
    }

    /* Check header */
    if ((sys_hdr.tot_len > SYS_STATUS_AREA_SZ) || (sys_hdr.tot_len < SYS_STATUS_HEADER_SZ)) {
        status = SYS_STATUS_ERR_TOTAL_LEN;
        goto exit;
    }
    if (sys_hdr.magic != SYS_STATUS_MAGIC_CODE) {
        status = SYS_STATUS_ERR_MAGIC;
        goto exit;
    }

    /* Get active system status */
    if (sys_hdr.tot_len % AES_BLOCK_SZ) {
        alen = ((sys_hdr.tot_len >> 4) << 4) + AES_BLOCK_SZ;
    } else {
        alen = sys_hdr.tot_len;
    }
    buf = mbedtls_calloc(alen, 1);
    if (NULL == buf) {
        status = SYS_STATUS_ERR_MEM;
        goto exit;
    }
    ret = flash_indirect_read(flash_offset, buf, alen);
    if (ret != 0) {
        status = SYS_STATUS_ERR_FLASH;
        goto exit;
    }
    ret = sys_status_crypt(MBEDTLS_AES_DECRYPT, buf, alen, buf);
    if (ret != 0) {
        status = SYS_STATUS_ERR_CRYPT;
        goto exit;
    }

    /* Check checksum */
    if (checksum_cal(buf, sys_hdr.tot_len) != 0) {
        status = SYS_STATUS_ERR_CHECKSUM;
        goto exit;
    }

    /* Check if the TLVs are correct */
    p = buf + SYS_STATUS_HEADER_SZ;
    sz = sys_hdr.tot_len - SYS_STATUS_HEADER_SZ;
    while (sz > 0) {
        t = *p; l = *(p + 1);
        if ((t == SYS_UNKNOWN_TYPE) || (l > MAX_TLV_VALUE_SIZE)) {
            status = SYS_STATUS_ERR_TLV;
            goto exit;
        }
        p += SYS_STATUS_TLV_HEADER_SZ + l;
        sz -= SYS_STATUS_TLV_HEADER_SZ + l;
    }
    if (sz != 0) {
        status = SYS_STATUS_ERR_TLV;
        goto exit;
    }

    status = SYS_STATUS_OK;

exit:
    if (buf)
        mbedtls_free(buf);
    return status;
}

/*!
    \brief      set inernal system status
    \param[in]  type: type of system status
    \param[in]  len: length of system status
    \param[in]  pval: pointer to the value of system status
    \param[out] none
    \retval     status of set system internal
      \arg        SYS_STATUS_OK: status is ok
      \arg        SYS_STATUS_ERR_TOTAL_LEN: total length error
      \arg        SYS_STATUS_ERR_MAGIC: magic error
      \arg        SYS_STATUS_ERR_MEM: memory error
      \arg        SYS_STATUS_ERR_FLASH: flash error
      \arg        SYS_STATUS_ERR_CRYPT: crypt error
      \arg        SYS_STATUS_ERR_CHECKSUM: checksum error
      \arg        SYS_STATUS_ERR_TLV: tlv error
*/
static int sys_status_internal_set(uint8_t type, uint8_t len, uint8_t* pval)
{
    struct sys_status_header_t sys_hdr, *hdr;
    uint32_t start, newstart, tlen, nlen, olen;
    uint8_t *buf = NULL, *p;
    int loc, ret, status;

    if ((len > MAX_TLV_VALUE_SIZE) || (len == 0)) {
        status = SYS_STATUS_ERR_INPUT;
        goto exit1;
    }
    mbl_trace(MBL_DBG, "Set sys status type %d.\r\n", type);

    /* Found the active system status */
    status = active_header_get(&sys_hdr, &start);
    if (status != SYS_STATUS_OK) {
        goto exit;
    }

    /* Check header */
    if (sys_hdr.tot_len > SYS_STATUS_AREA_SZ) {
        status = SYS_STATUS_ERR_TOTAL_LEN;
        goto exit;
    }
    if (sys_hdr.magic != SYS_STATUS_MAGIC_CODE) {
        status = SYS_STATUS_ERR_MAGIC;
        goto exit;
    }

    /* Get active system status */
    if (sys_hdr.tot_len % AES_BLOCK_SZ) {
        tlen = ((sys_hdr.tot_len >> 4) << 4) + AES_BLOCK_SZ;
    } else {
        tlen = sys_hdr.tot_len;
    }
    nlen = sys_hdr.tot_len + SYS_STATUS_TLV_HEADER_SZ + len;
    if (nlen % AES_BLOCK_SZ) {
        nlen = ((nlen >> 4) << 4) + AES_BLOCK_SZ;
    }
    buf = mbedtls_calloc(nlen, 1);  // consider new tlv entry
    if (NULL == buf) {
        status = SYS_STATUS_ERR_MEM;
        goto exit;
    }
    ret = flash_indirect_read(start, buf, tlen);
    if (ret != 0) {
        status = SYS_STATUS_ERR_FLASH;
        goto exit;
    }
    ret = sys_status_crypt(MBEDTLS_AES_DECRYPT, buf, tlen, buf);
    if (ret != 0) {
        status = SYS_STATUS_ERR_CRYPT;
        goto exit;
    }

    /* Add or update TLV */
    loc = tlv_find_by_type((buf + SYS_STATUS_HEADER_SZ), (sys_hdr.tot_len - SYS_STATUS_HEADER_SZ), type); /* Check if found */
    if (loc >= 0) {  // already existed, need to update
        olen = *(buf + AES_BLOCK_SZ + loc + 1);
        if (olen == len) {
            p = buf + AES_BLOCK_SZ + loc + SYS_STATUS_TLV_HEADER_SZ;
            memcpy(p, pval, olen);
        } else {
            status = SYS_STATUS_ERR_TLV;
            goto exit;
        }
    } else {  // new tlv
        if (sys_hdr.tot_len + SYS_STATUS_TLV_HEADER_SZ + len > SYS_STATUS_AREA_SZ) {
            status = SYS_STATUS_ERR_TOTAL_LEN;
            goto exit;
        }
        p = buf + sys_hdr.tot_len;
        *p++ = type;
        *p++ = len;
        memcpy(p, pval, len);
        p += len;
        /* padding with 0xFF for 16-bytes alignment */
        memset(p, 0xFF, (nlen - ((uint32_t)p - (uint32_t)buf)));
        /* update total length */
        sys_hdr.tot_len += SYS_STATUS_TLV_HEADER_SZ + len;
        tlen = nlen;
    }

    /* Update header */
    hdr = (struct sys_status_header_t *)buf;
    hdr->tot_len = sys_hdr.tot_len;
    if (sys_hdr.act_cntr == 0xFFFFFFFE)
        hdr->act_cntr = 0;
    else
        hdr->act_cntr = sys_hdr.act_cntr + 1;
    hdr->checksum = 0;
    hdr->checksum = checksum_cal(buf, hdr->tot_len);

    /* Encrypt it and write to another entry */
    ret = sys_status_crypt(MBEDTLS_AES_ENCRYPT, buf, tlen, buf);
    if (ret != 0) {
        status = SYS_STATUS_ERR_CRYPT;
        goto exit;
    }
    if (start == FLASH_OFFSET_SYS_STATUS_PING)
        newstart = FLASH_OFFSET_SYS_STATUS_PONG;
    else
        newstart = FLASH_OFFSET_SYS_STATUS_PING;
    ret = flash_erase(newstart, SYS_STATUS_AREA_SZ);
    if (ret != 0) {
        status = SYS_STATUS_ERR_FLASH;
        goto exit;
    }
    ret = flash_write((newstart + AES_BLOCK_SZ), (buf + AES_BLOCK_SZ), (tlen - AES_BLOCK_SZ));
    if (ret != 0) {
        status = SYS_STATUS_ERR_FLASH;
        goto exit;
    }
    ret = flash_write(newstart, buf, AES_BLOCK_SZ);
    if (ret != 0) {
        status = SYS_STATUS_ERR_FLASH;
        goto exit;
    }

    status = SYS_STATUS_OK;

exit:
    if (buf)
        mbedtls_free(buf);
exit1:
    if (status != SYS_STATUS_OK)
        mbl_trace(MBL_WARN, "Status set failed(0x%x,%d).\r\n", status, ret);
    return status;
}

/*!
    \brief      set system status
    \param[in]  type: type of system status
    \param[in]  len: length of system status
    \param[in]  pval: pointer to the value of system status
    \param[out] none
    \retval     status of set system
      \arg        -1: version error
      \arg        -2: image status error
      \arg        SYS_STATUS_OK: status is ok
      \arg        SYS_STATUS_ERR_TOTAL_LEN: total length error
      \arg        SYS_STATUS_ERR_MAGIC: magic error
      \arg        SYS_STATUS_ERR_MEM: memory error
      \arg        SYS_STATUS_ERR_FLASH: flash error
      \arg        SYS_STATUS_ERR_CRYPT: crypt error
      \arg        SYS_STATUS_ERR_CHECKSUM: checksum error
      \arg        SYS_STATUS_ERR_TLV: tlv error
*/
int sys_status_set(uint8_t type, uint8_t len, uint8_t* pval)
{
    if (type == SYS_IMG_VER_COUNTER) {
        mbl_trace(MBL_WARN, "Please call sys_update_fw_version to set version.\r\n");
        return -1;
    }
    if ((type == SYS_IMAGE0_STATUS) || (type == SYS_IMAGE1_STATUS)) {
        mbl_trace(MBL_WARN, "Please call sys_img_flag_set to set image status.\r\n");
        return -2;
    }

    return sys_status_internal_set(type, len, pval);
}

/*!
    \brief      get system status
    \param[in]  type: type of system status
    \param[in]  len: length of system status
    \param[out] pval: pointer to the value of system status
    \retval     status of get system
      \arg        SYS_STATUS_FOUND_OK: system status found ok
      \arg        SYS_STATUS_NOT_FOUND: system status not found
      \arg        SYS_STATUS_FOUND_ERR: system status found error
*/
int sys_status_get(uint8_t type, uint8_t len, uint8_t* pval)
{
    struct sys_status_header_t sys_hdr;
    uint32_t start, tlen;
    uint8_t *buf = NULL, *p;
    int loc, ret, status;

    /* Found the active system status */
    ret = active_header_get(&sys_hdr, &start);
    if (ret != SYS_STATUS_OK) goto exit;

    /* Check header */
    if (sys_hdr.tot_len > SYS_STATUS_AREA_SZ) {
        ret = SYS_STATUS_ERR_TOTAL_LEN;
        goto exit;
    }
    if (sys_hdr.magic != SYS_STATUS_MAGIC_CODE) {
        ret = SYS_STATUS_ERR_MAGIC;
        goto exit;
    }

    /* Get active system status */
    if (sys_hdr.tot_len % AES_BLOCK_SZ) {
        tlen = ((sys_hdr.tot_len >> 4) << 4) + AES_BLOCK_SZ;
    } else {
        tlen = sys_hdr.tot_len;
    }
    buf = mbedtls_calloc(tlen, 1);
    if (NULL == buf) goto exit;

    ret = flash_indirect_read(start, buf, tlen);
    if (ret != 0) goto exit;

    ret = sys_status_crypt(MBEDTLS_AES_DECRYPT, buf, tlen, buf);
    if (ret != 0) goto exit;

    /* Find item with the type */
    loc = tlv_find_by_type((buf + SYS_STATUS_HEADER_SZ), (sys_hdr.tot_len - SYS_STATUS_HEADER_SZ), type); /* Check if found */
    if (loc >= 0) {
        /* Found */
        p = buf + SYS_STATUS_HEADER_SZ + loc;
        if (*(p + 1) > len)
            memcpy(pval, (p + SYS_STATUS_TLV_HEADER_SZ), len);
        else
            memcpy(pval, (p + SYS_STATUS_TLV_HEADER_SZ), *(p + 1));
        status = SYS_STATUS_FOUND_OK;
    } else {
        /* Not found */
        status = SYS_STATUS_NOT_FOUND;
        memset(pval, 0xFF, len);
    }

    mbedtls_free(buf);
    return status;

exit:
    if (buf)
        mbedtls_free(buf);
    memset(pval, 0xFF, len);
    status = SYS_STATUS_FOUND_ERR;
    mbl_trace(MBL_WARN, "Status get failed(0x%x,%d).\r\n", status, ret);
    return status;
}

/*!
    \brief      set system error process
    \param[in]  method: method of process system error
    \param[out] none
    \retval     status of set system error process
      \arg        -1: method error
      \arg        SYS_STATUS_OK: status is ok
      \arg        SYS_STATUS_ERR_TOTAL_LEN: total length error
      \arg        SYS_STATUS_ERR_MAGIC: magic error
      \arg        SYS_STATUS_ERR_MEM: memory error
      \arg        SYS_STATUS_ERR_FLASH: flash error
      \arg        SYS_STATUS_ERR_CRYPT: crypt error
      \arg        SYS_STATUS_ERR_CHECKSUM: checksum error
      \arg        SYS_STATUS_ERR_TLV: tlv error
*/
int sys_err_process_set(uint8_t method)
{
    if (method > 1) {
        return -1;
    }
    return sys_status_internal_set(SYS_ERROR_PROCESS, LEN_SYS_ERROR_PROCESS, &method);
}

/*!
    \brief      set system trace level
    \param[in]  trace_level: trace level of system
    \param[out] none
    \retval     status of set system error process
      \arg        -1: trace level error
      \arg        SYS_STATUS_OK: status is ok
      \arg        SYS_STATUS_ERR_TOTAL_LEN: total length error
      \arg        SYS_STATUS_ERR_MAGIC: magic error
      \arg        SYS_STATUS_ERR_MEM: memory error
      \arg        SYS_STATUS_ERR_FLASH: flash error
      \arg        SYS_STATUS_ERR_CRYPT: crypt error
      \arg        SYS_STATUS_ERR_CHECKSUM: checksum error
      \arg        SYS_STATUS_ERR_TLV: tlv error
*/
int sys_trace_level_set(uint8_t trace_level)
{
    if (trace_level > MBL_DBG) {
        return -1;
    }
    return sys_status_internal_set(SYS_TRACE_LEVEL, LEN_SYS_TRACE_LEVEL, &trace_level);
}

/*!
    \brief      set system image flag
    \param[in]  inx: image index
    \param[in]  mask: image status mask value
    \param[in]  flag: image flag
    \param[out] none
    \retval     status of set image flag
      \arg        SYS_STATUS_OK: status is ok
      \arg        SYS_STATUS_ERR_TOTAL_LEN: total length error
      \arg        SYS_STATUS_ERR_MAGIC: magic error
      \arg        SYS_STATUS_ERR_MEM: memory error
      \arg        SYS_STATUS_ERR_FLASH: flash error
      \arg        SYS_STATUS_ERR_CRYPT: crypt error
      \arg        SYS_STATUS_ERR_CHECKSUM: checksum error
      \arg        SYS_STATUS_ERR_TLV: tlv error
      \arg        -3: image index error
      \arg        -4: image status error
*/
int sys_img_flag_set(uint8_t idx, uint8_t mask, uint8_t flag)
{
    uint8_t type, img_status = 0;
    int ret;

    if (idx == IMAGE_0)
        type = SYS_IMAGE0_STATUS;
    else if (idx == IMAGE_1)
        type = SYS_IMAGE1_STATUS;
    else {
        ret = -3;
        goto exit;
    }

    ret = sys_status_get(type, LEN_SYS_IMAGE_STATUS, &img_status);
    if ((ret != SYS_STATUS_NOT_FOUND) && (ret != SYS_STATUS_FOUND_OK)) {
        goto exit;
    }
    if (ret == SYS_STATUS_NOT_FOUND)
        img_status = 0;

    /* the flag bits have been set in img_status */
    if ((img_status & mask) == flag) {
        ret = SYS_STATUS_OK;
        goto exit;
    }

    img_status = ((img_status & ~mask) | flag);

    if (!image_valid_status_get(img_status)) {
        ret = -4;
        goto exit;
    }

    ret = sys_status_internal_set(type, LEN_SYS_IMAGE_STATUS, &img_status);

exit:
    return ret;
}

/*!
    \brief      set system running image
    \param[in]  idx: image index
    \param[out] none
    \retval     status of set running image
      \arg        SYS_STATUS_OK: status is ok
      \arg        SYS_STATUS_ERR_TOTAL_LEN: total length error
      \arg        SYS_STATUS_ERR_MAGIC: magic error
      \arg        SYS_STATUS_ERR_MEM: memory error
      \arg        SYS_STATUS_ERR_FLASH: flash error
      \arg        SYS_STATUS_ERR_CRYPT: crypt error
      \arg        SYS_STATUS_ERR_CHECKSUM: checksum error
      \arg        SYS_STATUS_ERR_TLV: tlv error
      \arg        -3: image index error
*/
int sys_running_img_set(uint8_t idx)
{
    uint8_t img_idx = 0xff;
    int ret;

    ret = sys_status_get(SYS_RUNNING_IMG, LEN_SYS_RUNNING_IMG, &img_idx);
    if ((ret != SYS_STATUS_NOT_FOUND) && (ret != SYS_STATUS_FOUND_OK)) {
        goto exit;
    }

    /* the index to set is same as current running index */
    if (img_idx == idx) {
        ret = SYS_STATUS_OK;
        goto exit;
    }

    if ((idx == IMAGE_0) || (idx == IMAGE_1))
        ret = sys_status_internal_set(SYS_RUNNING_IMG, LEN_SYS_RUNNING_IMG, &idx);
    else {
        ret = -3;
    }
exit:
    return ret;
}

/*!
    \brief      set system firmware version
    \param[in]  type: image type
    \param[in]  version: image version
    \param[out] none
    \retval     status of set firmware version
      \arg        SYS_STATUS_OK: status is ok
      \arg        SYS_STATUS_ERR_TOTAL_LEN: total length error
      \arg        SYS_STATUS_ERR_MAGIC: magic error
      \arg        SYS_STATUS_ERR_MEM: memory error
      \arg        SYS_STATUS_ERR_FLASH: flash error
      \arg        SYS_STATUS_ERR_CRYPT: crypt error
      \arg        SYS_STATUS_ERR_CHECKSUM: checksum error
      \arg        SYS_STATUS_ERR_TLV: tlv error
      \arg        -1: image type error
*/
int sys_fw_version_set(uint32_t type, uint32_t version)
{
    uint8_t status_type;
    uint32_t local_ver = 0;
    int ret;

    if (type != IMG_TYPE_IMG)
        return -1;
    else
        status_type = SYS_IMG_VER_COUNTER;

    ret = sys_status_get(status_type, LEN_SYS_VER_COUNTER, (uint8_t *)&local_ver);
    if ((ret != SYS_STATUS_NOT_FOUND) && (ret != SYS_STATUS_FOUND_OK)) {
        return ret;
    }
    if (ret == SYS_STATUS_NOT_FOUND)
        local_ver = 0;

    /* New version MUST BE higher than local */
    if (version <= local_ver) {
        return SYS_STATUS_OK;
    }

    return sys_status_internal_set(status_type, LEN_SYS_VER_COUNTER, (uint8_t *)&version);
}

/*!
    \brief      set system trng seed
    \param[in]  val: trng seed value
    \param[out] none
    \retval     status of set system trng
      \arg        SYS_STATUS_OK: status is ok
      \arg        SYS_STATUS_ERR_TOTAL_LEN: total length error
      \arg        SYS_STATUS_ERR_MAGIC: magic error
      \arg        SYS_STATUS_ERR_MEM: memory error
      \arg        SYS_STATUS_ERR_FLASH: flash error
      \arg        SYS_STATUS_ERR_CRYPT: crypt error
      \arg        SYS_STATUS_ERR_CHECKSUM: checksum error
      \arg        SYS_STATUS_ERR_TLV: tlv error
*/
int sys_trng_seed_set(uint8_t val)
{
    uint8_t is_trng_seed = val ? 1 : 0;
    return sys_status_internal_set(SYS_TRNG_SEED, LEN_SYS_TRNG_SEED, &is_trng_seed);
}

/*!
    \brief      show system status
    \param[in]  none
    \param[out] none
    \retval     none
*/
void sys_status_show(void)
{
    struct sys_status_header_t sys_hdr;
    uint32_t start, tlen, offset = 0, sz;
    uint8_t *buf = NULL, *p;
    uint8_t t, l, *v;
    char *title;
    int ret;

    /* Found the active system status */
    ret = active_header_get(&sys_hdr, &start);
    if (ret != SYS_STATUS_OK) goto exit;

    /* Check header */
    if ((sys_hdr.tot_len > SYS_STATUS_AREA_SZ) || (sys_hdr.magic != SYS_STATUS_MAGIC_CODE)) {
        ret = -0xFF;
        goto exit;
    }

    if (start == FLASH_OFFSET_SYS_STATUS_PING)
        title = "System Status: Ping\r\n";
    else
        title = "System Status: Pong\r\n";

    printf("%s=============================\r\n", title);
    printf("Total Length: 0x%x (%d)\t\r\n", sys_hdr.tot_len, sys_hdr.tot_len);
    printf("Active Counter: 0%x\t\r\n", sys_hdr.act_cntr);
    printf("Checksum: 0x%x\t\t\r\n", sys_hdr.checksum);

    /* Get active system status */
    if (sys_hdr.tot_len % AES_BLOCK_SZ) {
        tlen = ((sys_hdr.tot_len >> 4) << 4) + AES_BLOCK_SZ;
    } else {
        tlen = sys_hdr.tot_len;
    }
    buf = mbedtls_calloc(tlen, 1);
    if (NULL == buf) goto exit;

    ret = flash_indirect_read(start, buf, tlen);
    if (ret != 0) goto exit;

    ret = sys_status_crypt(MBEDTLS_AES_DECRYPT, buf, tlen, buf);
    if (ret != 0) goto exit;

    printf("T\tL\tV\r\n");

    /* List types. */
    p = buf + SYS_STATUS_HEADER_SZ;
    sz = sys_hdr.tot_len - SYS_STATUS_HEADER_SZ;
    while (offset < sz) {
        t = *p;  l = *(p + 1);  v = p + 2;
        tlv_print(t, l, v);
        p += SYS_STATUS_TLV_HEADER_SZ + l;
        offset += SYS_STATUS_TLV_HEADER_SZ + l;
    }

exit:
    if (buf)
        mbedtls_free(buf);
    if (ret != 0)
        mbl_trace(MBL_WARN, "Status show error(0x%x).\r\n", ret);
    return;
}

/*!
    \brief      dump system status
    \param[in]  is_ping: check system is ping or pong
    \param[out] none
    \retval     none
*/
void sys_status_dump(int is_ping)
{
    struct sys_status_header_t *sys_hdr;
    uint32_t start, tlen;
    uint8_t blk[AES_BLOCK_SZ], *buf = NULL;
    char *title;
    int ret;

    if (is_ping ) {
        start = FLASH_OFFSET_SYS_STATUS_PING;
        title = "Status PING:\r\n";
    } else {
        start = FLASH_OFFSET_SYS_STATUS_PONG;
        title = "Status PONG:\r\n";
    }

    /* Read header */
    ret = flash_indirect_read(start, blk, AES_BLOCK_SZ);
    if (ret != 0) goto exit;

    ret = sys_status_crypt(MBEDTLS_AES_DECRYPT, blk, AES_BLOCK_SZ, blk);
    if (ret != 0) goto exit;

    sys_hdr = (struct sys_status_header_t *)blk;

    /* Check header */
    if ((sys_hdr->tot_len > SYS_STATUS_AREA_SZ) || (sys_hdr->magic != SYS_STATUS_MAGIC_CODE)) {
        ret = -0xFF;
        goto exit;
    }

    /* Allocate memory for decrypt TLVs */
    if (sys_hdr->tot_len % AES_BLOCK_SZ) {
        tlen = ((sys_hdr->tot_len >> 4) << 4) + AES_BLOCK_SZ;
    } else {
        tlen = sys_hdr->tot_len;
    }
    buf = mbedtls_calloc(tlen, 1);
    if (NULL == buf) goto exit;

    ret = flash_indirect_read(start, buf, tlen);
    if (ret != 0) goto exit;

    ret = sys_status_crypt(MBEDTLS_AES_DECRYPT, buf, tlen, buf);
    if (ret != 0) goto exit;

    mbl_print_data(title, buf, sys_hdr->tot_len);

exit:
    if (buf)
        mbedtls_free(buf);
    if (ret != 0)
        mbl_trace(MBL_WARN, "Status dump error(0x%x).\r\n", ret);
    return;
}

/*!
    \brief      check system status
    \param[in]  none
    \param[out] none
    \retval     status of check system
      \arg        0: check system status ok
      \arg        -1: initialize system status error
      \arg        -2: set system firmware version error
      \arg        -3: flash write error
      \arg        -4: check system status integrity error
*/
int sys_status_check(void)
{
    int ret;
    uint8_t val;
    uint32_t ver_lock_addr = FLASH_BASE + FLASH_OFFSET_IMG_VERSION_LOCK;  /* Last 4 bytes of the first 32k main bootloader area */
    uint32_t val32;

    /* Initialize mbedtls platform related function pointers */
    mbedtls_init();

    /* Set system status area as no decryption area */
    flash_nodec_config(0, (FLASH_OFFSET_SYS_STATUS_PING >> 12), (FLASH_OFFSET_SYS_STATUS_PING >> 12) + 1);

    /* init sys status */
    if (*(uint32_t*)ver_lock_addr == 0xFFFFFFFF) {
        ret = sys_status_init();
        if (ret != 0) {
            return -1;
        }
        val32 = 1;
        ret = sys_fw_version_set(IMG_TYPE_IMG, val32);
        if (ret != 0) {
            return -2;
        }
        val32 = 0xCDCDCDCD;
        ret = flash_write(FLASH_OFFSET_IMG_VERSION_LOCK, (void*)&val32, sizeof(uint32_t));
        if (ret != 0) {
            return -3;
        }
        mbl_trace(MBL_INFO, "Sys status init OK.\r\n");
    } else {
        ret = sys_statu_integritys_check();
        if (ret != 0) {
            mbl_trace(MBL_INFO, "Sys status check failed (%d).\r\n", ret);
            return -4;
        }
    }

    /* Read Trace Level */
    ret = sys_status_get(SYS_TRACE_LEVEL, LEN_SYS_TRACE_LEVEL, &val);
    if (ret >= SYS_STATUS_FOUND_OK){
        mbl_trace_level = val;
    }
    mbl_trace(MBL_INFO, "Sys status checked OK.\r\n");

    /* Read Error Process */
    ret = sys_status_get(SYS_ERROR_PROCESS, LEN_SYS_ERROR_PROCESS, &val);
    if (ret >= SYS_STATUS_FOUND_OK){
        if (val > ERR_PROCESS_REDOWNLOD)
            mbl_err_process = ERR_PROCESS_ENDLESS_LOOP;
        else
            mbl_err_process = ERR_PROCESS_REDOWNLOD;
    }

    return 0;
}
