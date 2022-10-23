/*!
    \file    rom_ibl_state.h
    \brief   Rom IBL state file for GD32W51x WiFi SDK

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

#ifndef __IBL_STATE_H__
#define __IBL_STATE_H__

#define IMPL_ID_MAX_SIZE (32u)

enum reset_flag_t {
	RESET_BY_UNKNOWN = 0,
	RESET_BY_BOR,
	RESET_BY_PIN,
	RESET_BY_PWR_ON,
	RESET_BY_SW,
	RESET_BY_FWDG,
	RESET_BY_WWDG,
	RESET_BY_LOW_PWR,
};

enum boot_status_t {
	BOOT_FAIL_UNKNOWN = -0xFF,
	BOOT_FAIL_BAD_SYS_CONF,
	BOOT_FAIL_NOT_FOUND_MBL,
	BOOT_FAIL_BAD_OPT,
	BOOT_FAIL_BAD_CERT,
	BOOT_FAIL_BAD_MBL,
	BOOT_FAIL_BAD_ENTRY,
#if (SYS_STATUS_ENCRPTED == 0)
	BOOT_FAIL_SET_INITIAL_VER,
#endif
	BOOT_FAIL_SET_NV_CNTR,
	BOOT_FAIL_ENABLE_FWDG,

	BOOT_START = 0,

	BOOT_HW_INIT_OK,
	BOOT_SYS_CONFIG_OK,
	BOOT_VERIFY_MBL_OK,
	BOOT_OK,
};

enum ibl_option_t {
	IBL_VERIFY_CERT_IMG = 0,
	IBL_VERIFY_IMG_ONLY = 2,
	IBL_VERIFY_NONE = 3,
};

struct sw_info_t {
	uint32_t type;						/* IMG_TYPE_XXX */
	uint32_t version;					/* The version read from image header */
#ifdef BOOT_RECORD
	uint8_t record[MAX_BOOT_RECORD_LEN];
	uint8_t record_len;
#else
	uint8_t signer_id[IMG_PK_LEN];	    /* The hash of Image public key */
	uint8_t digest[IMG_HASH_LEN];		/* The hash of Image digest (header + image self) */
#endif
};

struct ob_state_t {
	uint8_t rdp_lvl;					/* 0: Level_0; 1: Level_0.5; 2: Level_1 */
	uint8_t wp;							/* 1: First 32KB are write protected. Set valid after system reset. */
	uint8_t tzen;						/* 1: trustzone enabled; 0: trustzone disabled */
	uint8_t nqspi;						/* 1: SIP flash; 0: QSPI flash */
	uint8_t fmcob;						/* 1: Option Byte in FMC; 0: Option Byte from EFUSE */
	uint8_t rsvd[3];
};

/**
 * \struct ibl_state_t
 *
 * \brief Store the initial boot state for MBL
 */
struct ibl_state_t
{
	uint32_t reset_flag;			    /* enum reset_flag_t */
	int boot_status;					/* enum boot_status_t */
	uint32_t rom_ver;					/* Indicate the ROM version to SW for future use */
	uint32_t ibl_opt;					/* enum ibl_option_t */
	uint8_t impl_id[IMPL_ID_MAX_SIZE];	/* Implementation ID */
	struct ob_state_t obstat;			/* Option byte. Read from FMC. */
	uint8_t mbl_pk[ED25519_PK_LEN];		/* Use Curve25519 for image signature. Read from MBL Certificate. */
	struct sw_info_t mbl_info;			/* SW measurements: type, version, measurement */
};

void store_ibl_state(struct ibl_state_t *state);

#endif  /* __IBL_STATE_H__ */
