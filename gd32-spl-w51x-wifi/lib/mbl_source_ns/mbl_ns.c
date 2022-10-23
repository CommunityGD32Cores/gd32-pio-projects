/*!
    \file    mbl_ns.c
    \brief   Non-secure MBL for GD32W51x WiFi SDK

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
#include "mbl_region.h"
#include "mbl_image.h"
#include "mbl_sys.h"
#include "mbl_uart.h"
#include "mbl_flash.h"

typedef void (*img_fptr_t) (void);
int32_t mbl_err_process = ERR_PROCESS_ENDLESS_LOOP;
int32_t mbl_trace_level = MBL_WARN;

#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
/* Macros to pick linker symbols */
#if defined (__ICCARM__)
#define REGION(a, b) a##b
#define REGION_NAME(a, b) REGION(a, b)
#define REGION_DECLARE(a, b) extern uint32_t REGION_NAME(a, b)
REGION_DECLARE(CSTACK, $$Base);
#else /* __CC_ARM or __GNUC__ */
#define REGION(a, b, c) a##b##c
#define REGION_NAME(a, b, c) REGION(a, b, c)
#define REGION_DECLARE(a, b, c) extern uint32_t REGION_NAME(a, b, c)
REGION_DECLARE(Image$$, ARM_LIB_STACKHEAP, $$ZI$$Base);
#endif /* __ICCARM__ */
#endif

/*!
    \brief      jump to main image
    \param[in]  msp: top of stack pointer for main image
    \param[in]  reset: Reset Handler for main
    \param[out] none
    \retval     none
*/
static void jump_to_main_image(uint32_t msp, uint32_t reset)
{
    static img_fptr_t img_reset;

    img_reset = (img_fptr_t)reset;

#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
    /* Restore the Main Stack Pointer Limit register's reset value
     * before passing execution to runtime firmware to make the
     * bootloader transparent to it.
     */
    __set_MSPLIM(0);
#endif
    __set_MSP(msp);

    __DSB();
    __ISB();

    img_reset();
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int /*main*/ main_masterbootloader(void)
{
    struct image_header hdr;
    uint32_t boot_idx = 0, image_offset = 0, version;
    uint32_t arm_vector[2];
    int ret;
#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
    uint32_t msp_stack_bottom = 0;
    #if defined (__ICCARM__)
        msp_stack_bottom = (uint32_t)&REGION_NAME(CSTACK, $$Base);
    #else
        msp_stack_bottom = (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACKHEAP, $$ZI$$Base);
    #endif
    __set_MSPLIM(msp_stack_bottom);
#endif

    /* Clear SRAM used by rom variables */
    memset((uint32_t *)SRAM_BASE_NS, 0, RE_MBL_DATA_START);

    /* Initialize UART for trace print */
    log_uart_init();
    printf("GIGA DEVICE\r\n");

    /* Initialize flash for reading system info */
    flash_init();

    /* Initialize system status if empty or validate system status */
    ret = sys_status_check();
    if (ret < 0) {
        mbl_trace(MBL_ERR, "Check sys status failed (ret = %d).\r\n", ret);
        goto BootFailed;
    }

    /* Find the correct image to boot, Image 0 or Image 1 */
    ret = boot_image_find(&boot_idx, &image_offset);
    if (ret < 0) {
        mbl_trace(MBL_ERR, "No image to boot (ret = %d).\r\n", ret);
        goto BootFailed;
    } else {
        mbl_trace(MBL_ALWAYS, "MBL: Boot from Image %d.\r\n", boot_idx);
    }

    /* If boot from Image 1, config offset mapping */
    if (boot_idx == IMAGE_1) {
        flash_offset_mapping();
    }

#ifdef CONFIG_IMAGE_VERIFICATION
    /* Read and check if image header is valid */
    flash_read(image_offset, (void *)&hdr, sizeof(struct image_header));
    ret = image_header_check(&hdr);
    if (ret < 0) {
        mbl_trace(MBL_ERR, "Check image header failed (ret = %d).\r\n", ret);
        sys_img_flag_set(boot_idx, IMG_FLAG_VERIFY_MASK, IMG_FLAG_VERIFY_FAIL);
        goto BootFailed;
    }
#endif
    /* Update Image status and Running Image flag */
    ret = sys_img_flag_set(boot_idx, IMG_FLAG_VERIFY_MASK, IMG_FLAG_VERIFY_OK);
    ret |= sys_running_img_set(boot_idx);

#ifdef CONFIG_IMAGE_VERIFICATION
    /* Update image version counter */
    version = (hdr.ver_major << 24) | (hdr.ver_minor << 16) | hdr.ver_rev;
    ret = sys_fw_version_set(IMG_TYPE_IMG, version);
    if (ret < 0) {
        mbl_trace(MBL_ERR, "Update image version counter failed(%d).\r\n", ret);
        goto BootFailed;
    } else if (ret == 0){
        mbl_trace(MBL_ALWAYS, "Current image version is %d.%d.%d.\r\n",
                            hdr.ver_major, hdr.ver_minor, hdr.ver_rev);
    }
#endif
    /* Read the MSP and Reset_Handler of the main image */
    flash_read((image_offset + IMAGE_HEADER_SIZE), arm_vector, 8);
    if (!is_valid_flash_addr(arm_vector[1])) {
        mbl_trace(MBL_ERR, "Image vector reset is invalid (%08x).\r\n", arm_vector[1]);
        goto BootFailed;
    }
    log_uart_idle_wait();

    /* Jump to main image */
    jump_to_main_image(arm_vector[0], arm_vector[1]);

BootFailed:
    while(1);
}
