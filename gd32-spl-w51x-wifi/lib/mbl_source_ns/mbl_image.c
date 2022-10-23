/*!
    \file    mbl_image.c
    \brief   Non-secure MBL image file for GD32W51x WiFi SDK

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

#include "config_gdm32_ntz.h"
#include "mbl_trace.h"
#include "mbl_image.h"
#include "mbl_sys.h"

/*!
    \brief      calculate checksum
    \param[in]  ptr: array pointer to calculate checksum
    \param[in]  sz: array size to calculate checksum
    \param[out] none
    \retval     checksum value
*/
uint32_t checksum_cal(uint8_t *ptr, uint32_t sz)
{
    uint32_t chksum = 0;
    int i;

    /* should be 4 bytes align */
    sz = ((sz >> 2) << 2);

    for (i = 0; i < sz; i += 4) {
        chksum ^= *(uint32_t *)(ptr + i);
    }

    return chksum;
}

/*!
    \brief      find boot image
    \param[in]  none
    \param[out] idx: pointer index of boot image
    \param[out] image_offset: pointer offset of boot image
    \retval     result of find boot image
      \arg        0: find image success
      \arg        -1: offset of image is null
      \arg        -2: find image0 fail
      \arg        -3: find image1 fail
      \arg        -4: image1 has downloaded but image0 has not downloaded
      \arg        -5: other fail
*/
int boot_image_find(OUT uint32_t *idx, OUT uint32_t *image_offset)
{
    uint8_t img0_stat = 0, img1_stat = 0;
    uint8_t img0_found = 0, img1_found = 0;
    int boot_idx = -1;
    int ret, result = 0;

    if (NULL == image_offset) {
        result = -1;
        goto Failed;
    }

    ret = sys_status_get(SYS_IMAGE0_STATUS, LEN_SYS_IMAGE_STATUS, &img0_stat);
    if (ret <= SYS_STATUS_FOUND_ERR) {
        result = -2;
        goto Failed;
    } else if (ret == SYS_STATUS_FOUND_OK) {
        img0_found = 1;
    } else {  // SYS_STATUS_NOT_FOUND
        img0_found = 0;
    }
    ret = sys_status_get(SYS_IMAGE1_STATUS, LEN_SYS_IMAGE_STATUS, &img1_stat);
    if (ret <= SYS_STATUS_FOUND_ERR) {
        result = -3;
        goto Failed;
    } else if (ret == SYS_STATUS_FOUND_OK) {
        img1_found = 1;
    } else {  // SYS_STATUS_NOT_FOUND
        img1_found = 0;
    }

    /* Image0 never downloaded, Image1 should not be downloaded too. */
    if (!img0_found && img1_found) {
        result = -4;
        goto Failed;
    }

    if (!img0_found && !img1_found) {
        /* ISP newly downloaded */
        sys_img_flag_set(IMAGE_0, IMG_FLAG_NEWER_MASK, IMG_FLAG_NEWER);
        boot_idx = 0;
        goto ImgSelected;
    }

    if (img0_found && ((img0_stat & IMG_FLAG_NEWER_MASK) == IMG_FLAG_NEWER)) {
        if (((img0_stat & IMG_FLAG_VERIFY_MASK) != IMG_FLAG_VERIFY_FAIL)
            && ((img0_stat & IMG_FLAG_IA_MASK) != IMG_FLAG_IA_FAIL)) {
            boot_idx = 0;
            goto ImgSelected;
        }
    }

    if (img1_found && ((img1_stat & IMG_FLAG_NEWER_MASK) == IMG_FLAG_NEWER)) {
        if (((img1_stat & IMG_FLAG_VERIFY_MASK) != IMG_FLAG_VERIFY_FAIL)
            && ((img1_stat & IMG_FLAG_IA_MASK) != IMG_FLAG_IA_FAIL)) {
            boot_idx = 1;
            goto ImgSelected;
        }
    }

    if (img0_found && ((img0_stat & IMG_FLAG_VERIFY_MASK) == IMG_FLAG_VERIFY_OK)
        && ((img0_stat & IMG_FLAG_IA_MASK) == IMG_FLAG_IA_OK)) {
        boot_idx = 0;
        goto ImgSelected;
    }

    if (img1_found && ((img1_stat & IMG_FLAG_VERIFY_MASK) == IMG_FLAG_VERIFY_OK)
        && ((img1_stat & IMG_FLAG_IA_MASK) == IMG_FLAG_IA_OK)) {
        boot_idx = 1;
        goto ImgSelected;
    }

    if (boot_idx == -1) {
        result = -5;
        goto Failed;
    }

ImgSelected:
    *idx = boot_idx;
    *image_offset = RE_IMG_0_NSPE_OFFSET + RE_VTOR_ALIGNMENT - IMAGE_HEADER_SIZE;

    return 0;

Failed:
    return result;
}

/*!
    \brief      check image header
    \param[in]  hdr: pointer to image header
    \param[out] none
    \retval     check result of image header
      \arg        0: image header is ok
      \arg        -1: head magic for boundary check
      \arg        -2: image type error
      \arg        -3: hash algorithm error
      \arg        -5: verson error
      \arg        -6: checksum error
      \arg        -7: nv counter for image version not found error
      \arg        -8: image version too low error
*/
int image_header_check(IN struct image_header *hdr)
{
    uint32_t ver_nv, ver_img;
    uint32_t chksum;
    int ret = 0;

    if (hdr->magic_h != IMG_MAGIC_H) {
        return -1;
    }
    if (hdr->img_type != IMG_TYPE_IMG) {
        return -2;
    }
    if (hdr->algo_hash != IMG_HASH_SHA256) {
        return -3;
    }
    /*if (hdr->algo_sign!= IMG_SIG_ED25519) {
        return -4;
    }*/

    if ((hdr->ver_major > MAX_VER_MAJOR) || (hdr->ver_minor > MAX_VER_MINOR)) {
        return -5;
    }

    chksum = checksum_cal((uint8_t *)hdr, sizeof(struct image_header));
    if (chksum != 0) {
        return -6;
    }

    ver_img = (hdr->ver_major << 24) | (hdr->ver_minor << 16) | hdr->ver_rev;
    ret = sys_status_get(SYS_IMG_VER_COUNTER, LEN_SYS_VER_COUNTER, (uint8_t *)&ver_nv);
    mbl_trace(MBL_INFO, "IMG version: %d.%d.%d, Local: %d.%d.%d\r\n",
                    hdr->ver_major, hdr->ver_minor, hdr->ver_rev,
                    ((ver_nv >> 24) & 0xff), ((ver_nv >> 16) & 0xff), (ver_nv & 0xffff));
    if (ret != 0) {
        return -7;
    }
    if (ver_img < ver_nv) {
        mbl_trace(MBL_ERR, "The image version too low(0x%08x < 0x%08x).\r\n", ver_img, ver_nv);
        return -8;
    }

    return 0;
}
