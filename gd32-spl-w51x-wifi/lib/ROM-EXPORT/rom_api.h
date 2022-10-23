/*!
    \file    rom_api.h
    \brief   Rom API for GD32W51x WiFi SDK

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

#ifndef __ROM_API_H__
#define __ROM_API_H__

#define MAX_API_NUM 	256

struct rom_api_s_t {
	int (*printf)(const char *format, ...);
	int (*trace_ex)(uint32_t level, const char *fmt, ...);
	int (*rand)(unsigned char *output, unsigned int len);
	void (*set_mutex_func)(int tolock, void *func);

	uint32_t (*cal_checksum)(IN uint8_t *ptr, IN uint32_t sz);
	int (*img_verify_sign)(IN uint8_t *hash,
						IN uint32_t hlen,
						IN uint8_t *pk,
						IN uint8_t *sig);
	int (*img_verify_hash)(IN uint32_t faddr,  /* Flash Adress */
						IN uint32_t len,
						IN uint8_t *hash,
						IN uint32_t hlen,
						IN uint8_t *seed,
						IN int seed_len);
	int (*img_verify_hdr)(IN void *hdr,
					IN uint8_t img_type);

	int (*img_verify_pkhash)(IN uint8_t *pk,
				IN uint32_t klen,
				IN uint8_t *pkhash,
				IN uint32_t hlen);

	int (*img_validate)(IN uint32_t img_faddr,
                   IN uint8_t img_type,
                   IN uint8_t *pk,
                   OUT void *img_info);

	int (*cert_validate)(IN uint32_t crt_faddr,
					IN size_t crt_sz,
					IN uint8_t *verify_pk,
					OUT uint8_t *img_pk);

	int (*sys_setting_get)(void *settings);
	int (*sys_status_set)(uint8_t type, uint8_t len, uint8_t *pval);
	int (*sys_status_get)(uint8_t type, uint8_t len, uint8_t *pval);
	int (*sys_set_trace_level)(uint8_t trace_level);
	int (*sys_set_err_process)(uint8_t method);
	int (*sys_set_img_flag)(uint8_t idx, uint8_t mask, uint8_t flag);
	int (*sys_reset_img_flag)(uint8_t idx);
	int (*sys_set_running_img)(uint8_t idx);
	int (*sys_set_fw_version)(uint32_t type, uint32_t version);
	int (*sys_set_pk_version)(uint32_t type, uint8_t key_ver);
	int (*sys_set_trng_seed)(uint8_t val);

	int32_t (*is_valid_flash_offset)(uint32_t offset);
	int32_t (*is_valid_flash_addr)(uint32_t addr);
	uint32_t (*flash_total_size)(void);
	uint32_t (*flash_erase_size)(void);
	int (*flash_init)(void);
	int (*flash_read)(uint32_t addr, void *data, int len);
	int (*flash_write)(uint32_t addr, const void *data, int len);
	int (*flash_write_fast)(uint32_t addr, const void *data, int len);
	int (*flash_erase)(uint32_t addr, int len);
#if defined(PLATFORM_GDM32)
	void (*fmc_unlock)(void);
	void (*fmc_lock)(void);
	void (*fmc_flag_clear)(uint32_t fmc_flag);
	int (*fmc_page_erase)(uint32_t page);
	int (*fmc_mass_erase)(void);
	int (*fmc_word_program)(uint32_t address, uint32_t data);
	int (*fmc_continuous_program)(uint32_t address, const uint32_t *data, uint32_t size);
	void (*qspi_flash_config)(uint32_t clock_prescaler);
	int32_t (*qspi_flash_erase_sector)(uint32_t address);
	int32_t (*qspi_flash_read)(uint32_t address, void *data, uint32_t size);
	int32_t (*qspi_flash_program_page)(uint32_t address, const uint8_t *data, uint32_t size);

	uint8_t (*efuse_get_ctl)(void);
	int (*efuse_set_ctl)(uint8_t ctl);
	uint8_t (*efuse_get_tzctl)(void);
	int (*efuse_set_tzctl)(uint8_t tzctl);
	uint8_t (*efuse_get_fp)(void);
	int (*efuse_set_fp)(uint8_t fp);
	uint8_t (*efuse_get_usctl)(void);
	int (*efuse_set_usctl)(uint8_t usctl);
	int (*efuse_get_mcui)(uint8_t *mcui);
	int (*efuse_set_mcui)(uint8_t *mcui);
	int (*efuse_get_aeskey)(uint8_t *aeskey);
	int (*efuse_set_aeskey)(uint8_t *aeskey);
	int (*efuse_get_rotpk)(uint8_t *rotpk);
	int (*efuse_set_rotpk)(uint8_t *rotpk);
	int (*efuse_get_dbg_pwd)(uint8_t *pwd);
	int (*efuse_set_dbg_pwd)(uint8_t *pwd);
	int (*efuse_get_rss)(uint8_t *rss);
	int (*efuse_set_rss)(uint8_t *rss);
	int (*efuse_get_uid)(uint8_t *uid);
	int (*efuse_get_rf)(uint8_t *rf, uint32_t offset, uint32_t sz);
	int (*efuse_set_rf)(uint8_t *rf, uint32_t offset, uint32_t sz);
	int (*efuse_get_usdata)(uint8_t *usdata, uint32_t offset, uint32_t sz);
	int (*efuse_set_usdata)(uint8_t *usdata, uint32_t offset, uint32_t sz);
#endif  /* PLATFORM_GDM32 */

	int (*do_iak_getpub)(int type, int rsa_keysz,
							int ecc_gid, int key_format,
							uint8_t *output, uint32_t *len);
	int (*do_iak_sign)(int type, int rsa_keysz, int ecc_gid,
                   const uint8_t *hash, size_t hash_len,
                   uint8_t *sig, size_t *sig_len);
/* #if (ROM_VERSION >= V_1_1) */
	int (*do_symm_key_derive)(uint8_t *label, size_t label_sz,
						uint8_t *key, size_t key_len);
/* #endif (ROM_VERSION >= V_1_1) */
};

void rom_set_mutex_func(int tolock, void *func);
int sys_setting_get(void *settings);
int sys_set_err_process(uint8_t method);
int sys_set_trace_level(uint8_t trace_level);
int sys_set_img_flag(uint8_t idx, uint8_t mask, uint8_t flag);
int sys_reset_img_flag(uint8_t idx);
int sys_set_running_img(uint8_t idx);
int sys_set_fw_version(uint32_t type, uint32_t version);
int sys_set_pk_version(uint32_t type, uint8_t version);
int sys_set_trng_seed(uint8_t val);
int sys_status_set(uint8_t type, uint8_t len, uint8_t *pval);
int sys_status_get(uint8_t type, uint8_t len, uint8_t* pval);

uint8_t efuse_get_ctl(void);
int efuse_set_ctl(uint8_t ctl);
uint8_t efuse_get_tzctl(void);
int efuse_set_tzctl(uint8_t tzctl);
uint8_t efuse_get_fp(void);
int efuse_set_fp(uint8_t fp);
uint8_t efuse_get_usctl(void);
int efuse_set_usctl(uint8_t usctl);
int efuse_get_mcui(uint8_t *mcui);
int efuse_set_mcui(uint8_t *mcui);
int efuse_get_aeskey(uint8_t *aeskey);
int efuse_set_aeskey(uint8_t *aeskey);
int efuse_get_rotpk(uint8_t *rotpk);
int efuse_set_rotpk(uint8_t *rotpk);
int efuse_get_dbg_pwd(uint8_t *pwd);
int efuse_set_dbg_pwd(uint8_t *pwd);
int efuse_get_rss(uint8_t *rss);
int efuse_set_rss(uint8_t *rss);
int efuse_get_uid(uint8_t *uid);
int efuse_get_huk(uint8_t *huk);
int efuse_get_rf(uint8_t *rf, uint32_t offset, uint32_t sz);
int efuse_set_rf(uint8_t *rf, uint32_t offset, uint32_t sz);
int efuse_get_usdata(uint8_t *usdata, uint32_t offset, uint32_t sz);
int efuse_set_usdata(uint8_t *usdata, uint32_t offset, uint32_t sz);

int do_iak_getpub(int type, int rsa_keysz,
							int ecc_gid, int key_format,
							uint8_t *output, uint32_t *len);
int do_iak_sign(int type, int rsa_keysz, int ecc_gid,
                   const uint8_t *hash, size_t hash_len,
                   uint8_t *sig, size_t *sig_len);

int do_symm_key_derive(uint8_t *label, size_t label_sz,
					uint8_t *key, size_t key_len);
#endif  //__ROM_API_H__

