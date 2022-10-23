/**
 * \file config_rom.h
 *
 * \brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#ifndef MBEDTLS_CONFIG_ROM_H
#define MBEDTLS_CONFIG_ROM_H

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#ifdef MBEDTLS_ROM_TEST
#include "rom_def.h"
#include "rom_stdlib.h"
#else
#include <stddef.h>
#include <stdint.h>
#endif

#if !defined(PLATFORM_MSP_AN521)
#define CONFIG_HW_SECURITY_ENGINE
extern int mbedtls_hwpka_flag;
#include "rom_pka.h"
#endif

#include "rom_config.h"

#ifndef ED25519_C
#define ED25519_C
#endif

/**
 * \def MBEDTLS_DEBUG_C
 *
 * Enable the debug functions.
 *
 * Module:  library/debug.c
 * Caller:  library/ssl_cli.c
 *          library/ssl_srv.c
 *          library/ssl_tls.c
 *
 * This module provides debugging functions.
 */
#define MBEDTLS_DEBUG_C

/**
 * \def MBEDTLS_SSL_CACHE_C
 *
 * Enable simple SSL cache implementation.
 *
 * Module:  library/ssl_cache.c
 * Caller:
 *
 * Requires: MBEDTLS_SSL_CACHE_C
 */
#define MBEDTLS_SSL_CACHE_C

/* SSL Cache options */
//#define MBEDTLS_SSL_CACHE_DEFAULT_TIMEOUT       86400 /**< 1 day  */
//#define MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES      50 /**< Maximum entries in cache */



/* SSL options */
#define MBEDTLS_SSL_MAX_CONTENT_LEN             5120	//16384 /**< Maxium fragment length in bytes, determines the size of each of the two internal I/O buffers */

/**
 * Complete list of ciphersuites to use, in order of preference.
 *
 * \warning No dependency checking is done on that field! This option can only
 * be used to restrict the set of available ciphersuites. It is your
 * responsibility to make sure the needed modules are active.
 *
 * Use this to save a few hundred bytes of ROM (default ordering of all
 * available ciphersuites) and a few to a few hundred bytes of RAM.
 *
 * The value below is only an example, not the default.
 */
//#define MBEDTLS_SSL_CIPHERSUITES 		MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
										//MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA,MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA256,MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256


#include "check_config.h"

#endif /* MBEDTLS_CONFIG_H */
