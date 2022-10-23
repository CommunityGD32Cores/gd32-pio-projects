/*!
    \file    rom_export.h
    \brief   Rom export file for GD32W51x WiFi SDK

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

#ifndef __ROM_EXPORT_H
#define __ROM_EXPORT_H

#define V_1_0				0x1
#define V_1_1				0x101

#ifndef IN
#define IN
#define OUT
#endif

#ifndef PLATFORM_GDM32
#define PLATFORM_GDM32
#endif

#include "stdint.h"
#include "stddef.h"
#include "rom_trace.h"
#include "rom_efuse.h"
#include "rom_region.h"
#include "rom_image.h"
#include "rom_sys.h"
#include "rom_ibl_state.h"
#include "rom_api.h"

extern struct rom_api_s_t *p_rom_api_s;

#define rom_printf							p_rom_api_s->printf
#define rom_trace_ex						p_rom_api_s->trace_ex
#define rom_rand							p_rom_api_s->rand
#define rom_set_mutex_func					p_rom_api_s->set_mutex_func

#define rom_cal_checksum					p_rom_api_s->cal_checksum
#define rom_img_verify_sign					p_rom_api_s->img_verify_sign
#define rom_img_verify_hash					p_rom_api_s->img_verify_hash
#define rom_img_verify_hdr					p_rom_api_s->img_verify_hdr
#define rom_img_verify_pkhash				p_rom_api_s->img_verify_pkhash
#define rom_img_validate					p_rom_api_s->img_validate
#define rom_cert_validate					p_rom_api_s->cert_validate

#define rom_sys_setting_get					p_rom_api_s->sys_setting_get
#define rom_sys_status_set					p_rom_api_s->sys_status_set
#define rom_sys_status_get					p_rom_api_s->sys_status_get
#define rom_sys_set_trace_level				p_rom_api_s->sys_set_trace_level
#define rom_sys_set_err_process				p_rom_api_s->sys_set_err_process
#define rom_sys_set_img_flag				p_rom_api_s->sys_set_img_flag
#define rom_sys_reset_img_flag				p_rom_api_s->sys_reset_img_flag
#define rom_sys_set_running_img				p_rom_api_s->sys_set_running_img
#define rom_sys_set_fw_ver					p_rom_api_s->sys_set_fw_version
#define rom_sys_set_pk_ver					p_rom_api_s->sys_set_pk_version
#define rom_sys_set_trng_seed				p_rom_api_s->sys_set_trng_seed

#define rom_is_valid_flash_offset			p_rom_api_s->is_valid_flash_offset
#define rom_is_valid_flash_addr				p_rom_api_s->is_valid_flash_addr
#define rom_flash_total_size				p_rom_api_s->flash_total_size
#define rom_flash_erase_size				p_rom_api_s->flash_erase_size
#define rom_flash_init						p_rom_api_s->flash_init
#define rom_flash_read						p_rom_api_s->flash_read
#define rom_flash_write						p_rom_api_s->flash_write
#define rom_flash_write_fast				p_rom_api_s->flash_write_fast
#define rom_flash_erase						p_rom_api_s->flash_erase

#define rom_fmc_unlock						p_rom_api_s->fmc_unlock
#define rom_fmc_lock						p_rom_api_s->fmc_lock
#define rom_fmc_flag_clear					p_rom_api_s->fmc_flag_clear
#define rom_fmc_page_erase					p_rom_api_s->fmc_page_erase
#define rom_fmc_mass_erase					p_rom_api_s->fmc_mass_erase
#define rom_fmc_word_program				p_rom_api_s->fmc_word_program
#define rom_fmc_continuous_program			p_rom_api_s->fmc_continuous_program
#define rom_qspi_flash_config				p_rom_api_s->qspi_flash_config
#define rom_qspi_flash_erase_sector			p_rom_api_s->qspi_flash_erase_sector
#define rom_qspi_flash_read					p_rom_api_s->qspi_flash_read
#define rom_qspi_flash_program_page			p_rom_api_s->qspi_flash_program_page

#define rom_efuse_get_ctl					p_rom_api_s->efuse_get_ctl
#define rom_efuse_set_ctl					p_rom_api_s->efuse_set_ctl
#define rom_efuse_get_tzctl					p_rom_api_s->efuse_get_tzctl
#define rom_efuse_set_tzctl					p_rom_api_s->efuse_set_tzctl
#define rom_efuse_get_fp					p_rom_api_s->efuse_get_fp
#define rom_efuse_set_fp					p_rom_api_s->efuse_set_fp
#define rom_efuse_get_usctl					p_rom_api_s->efuse_get_usctl
#define rom_efuse_set_usctl					p_rom_api_s->efuse_set_usctl
#define rom_efuse_get_mcui					p_rom_api_s->efuse_get_mcui
#define rom_efuse_set_mcui					p_rom_api_s->efuse_set_mcui
#define rom_efuse_get_aeskey				p_rom_api_s->efuse_get_aeskey
#define rom_efuse_set_aeskey				p_rom_api_s->efuse_set_aeskey
#define rom_efuse_get_rotpk					p_rom_api_s->efuse_get_rotpk
#define rom_efuse_set_rotpk					p_rom_api_s->efuse_set_rotpk
#define rom_efuse_get_dbg_pwd				p_rom_api_s->efuse_get_dbg_pwd
#define rom_efuse_set_dbg_pwd				p_rom_api_s->efuse_set_dbg_pwd
#define rom_efuse_get_rss					p_rom_api_s->efuse_get_rss
#define rom_efuse_set_rss					p_rom_api_s->efuse_set_rss
#define rom_efuse_get_uid					p_rom_api_s->efuse_get_uid
#define rom_efuse_get_rf					p_rom_api_s->efuse_get_rf
#define rom_efuse_set_rf					p_rom_api_s->efuse_set_rf
#define rom_efuse_get_usdata				p_rom_api_s->efuse_get_usdata
#define rom_efuse_set_usdata				p_rom_api_s->efuse_set_usdata

#define rom_do_iak_getpub					p_rom_api_s->do_iak_getpub
#define rom_do_iak_sign						p_rom_api_s->do_iak_sign
/*#if (ROM_VERSION >= V_1_1)*/
#define rom_do_symm_key_derive				p_rom_api_s->do_symm_key_derive
/*#endif (ROM_VERSION >= V_1_1) */
#endif  // __ROM_EXPORT_H
