/*!
    \file    rom_region.h
    \brief   Rom region file for GD32W51x WiFi SDK

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

#ifndef __ROM_REGION_H__
#define __ROM_REGION_H__

//#define ROM_SELF_TEST

#if defined(PLATFORM_MSP_AN521)
/* Flash: system settings and system status */
#define FLASH_OFFSET_SYS_SETTING		(0x00060000)   /* RE_SYS_SET_OFFSET */

/* ROM: code and ro data */
#define ROM_BASE_IBL					(0x10000000)
#define ROM_BASE_LIB					(ROM_BASE_IBL + IBL_CODE_SIZE)
#define IBL_CODE_START					ROM_BASE_IBL

/* SRAM: STACK, HEAP and other Global varaiables */
#define SRAM_BASE_ADDRESS				(0x38000000)

#elif defined(PLATFORM_GDM32)
/* Flash: system settings and system status */
#define FLASH_OFFSET_SYS_SETTING		0x0

#ifdef ROM_SELF_TEST
/* ROM: code and ro data */
#define ROM_BASE_IBL					(0x30000000)
#define ROM_BASE_LIB					(ROM_BASE_IBL + IBL_CODE_SIZE)

#define IBL_CODE_START					ROM_BASE_IBL
#define IBL_CODE_SIZE					(0x8000)
#define ROM_LIB_START					ROM_BASE_LIB
#define ROM_LIB_SIZE					(0x80000)

/* SRAM: STACK, HEAP and other Global varaiables */
#define SRAM_BASE_ADDRESS				(0x30060000)

#else
/* ROM: code and ro data */
#define ROM_BASE_IBL					(0x0FF84000) /* SECBOOT_BASE */
#define ROM_BASE_LIB					(0x0FF4F000) /* ROMLIB_BASE */

#define IBL_CODE_START					ROM_BASE_IBL
#define IBL_CODE_SIZE					(0x4000)    /* MAX 16K */
#define ROM_LIB_START					ROM_BASE_LIB
#define ROM_LIB_SIZE					(0x31000)   /* MAX 196K */

/* SRAM: STACK, HEAP and other Global varaiables */
#define SRAM_BASE_ADDRESS				(0x30000000)

#endif  /* ROM_SELF_TEST */
#endif  /* PLATFORM_GDM32 */

#define ROM_API_ARRAY_BASE				(ROM_LIB_START)
#define ROM_API_ARRAY_RSVD				0x800

#define IBL_DATA_START					SRAM_BASE_ADDRESS
#define IBL_DATA_SIZE					0x200
#define IBL_HEAP_SIZE					0x7000
#define IBL_MSP_STACK_SIZE				0x3000

/* SRAM: shared SRAM, store initial boot state */
#define IBL_SHARED_DATA_START			(IBL_DATA_START + IBL_DATA_SIZE)
#define IBL_SHARED_DATA_SIZE			0x800

/* Macros to pick linker symbols */
#define REGION(a, b, c) 				a##b##c
#define REGION_NAME(a, b, c) 			REGION(a, b, c)
#define REGION_DECLARE(a, b, c) 		extern uint32_t REGION_NAME(a, b, c)

#endif  // __ROM_REGION_H__
