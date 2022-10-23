/*!
    \file    mbl_image.h
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

#ifndef __MBL_IMAGE_H__
#define __MBL_IMAGE_H__

#define IN
#define OUT

/* Image Index */
#define IMAGE_0                   0
#define IMAGE_1                   1

/* Image Magic */
#define IMG_MAGIC_H               0x96f3b83d

/* Image type */
#define IMG_TYPE_MBL              0x1
#define IMG_TYPE_IMG              0x8

/* Image Hash Algorithm */
#define IMG_HASH_SHA256           0x1
#define IMG_HASH_SHA512           0x2

/* Image status */
#define IMG_FLAG_NEWER_MASK       0x01
#define IMG_FLAG_VERIFY_MASK      0x06
#define IMG_FLAG_IA_MASK          0x18

#define IMG_FLAG_OLDER            0x0
#define IMG_FLAG_NEWER            0x1         /* The image with higher version will be set to NEWER.
                                                Default Newer after ISP. Set or Cleared when New image downloaded though OTA.
                                                Checked when MBL finding the boot image.
                                                Only one image is set to be NEWER at the same time. */

#define IMG_FLAG_VERIFY_NONE      (0x0 << 1)  /* Default None. Set after MBL verification finished. Checked when MBL finding the boot image. */
#define IMG_FLAG_VERIFY_OK        (0x1 << 1)
#define IMG_FLAG_VERIFY_FAIL      (0x2 << 1)
#define IMG_FLAG_IA_NONE          (0x0 << 3)  /* Default None. Set after IA finished. Checked when MBL finding the boot image. */
#define IMG_FLAG_IA_OK            (0x1 << 3)
#define IMG_FLAG_IA_FAIL          (0x2 << 3)

/* Image version */
#define MAX_VER_MAJOR             254  // 16
#define MAX_VER_MINOR             254  // 16

/* Image header size */
#ifdef CONFIG_IMAGE_VERIFICATION
#define IMAGE_HEADER_SIZE         sizeof(struct image_header)
#else
#define IMAGE_HEADER_SIZE         0
#endif

/** Image header.  All fields are in little endian byte order. Total 32 bytes. */
struct image_header {
    uint32_t magic_h;                /* Head Magic for boundary check. */
    uint32_t tot_sz;                 /* The total size of Image, including header, TLVs and the cert if existed. */
    uint8_t mani_ver;                /* The version of Image Manifest Format. */
    uint8_t img_type;                /* Always NSPE. The type of Image (Firmware). */
    uint8_t algo_hash;               /* The Hash algorithm for Image digest */
    uint8_t algo_sign;               /* Not used when TZEN is 0. The algorithm used to sign Image manifest. */
    uint16_t hdr_sz;                 /* Size of Image Header (bytes). */
    uint16_t ptlv_sz;                /* Size of PTLVs (bytes). */
    uint32_t img_sz;                 /* Size of Image itself (bytes). */
    uint8_t ver_major;               /* Major Version. */
    uint8_t ver_minor;               /* Minor Version. */
    uint16_t ver_rev;                /* Revision. */
    uint32_t rsvd;                   /* Reserved. */
    uint32_t chksum;                 /* Header check sum. */
};

int boot_image_find(OUT uint32_t *idx, OUT uint32_t *image_offset);
int image_header_check(IN struct image_header *hdr);
uint32_t checksum_cal(uint8_t *ptr, uint32_t sz);

#endif  /* __MBL_IMAGE_H__ */
