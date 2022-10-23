/*!
    \file    rom_export_mbedtls.h
    \brief   Rom MbedTLS export file for GD32W51x WiFi SDK

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

#ifndef __ROM_EXPORT_MBEDTLS_H__
#define __ROM_EXPORT_MBEDTLS_H__

#include "mbedtls\md5.h"
#include "mbedtls\sha1.h"
#include "mbedtls\sha256.h"
#include "mbedtls\sha512.h"
#include "mbedtls\md.h"
#include "mbedtls\cipher.h"
#include "mbedtls\ssl_ciphersuites.h"
#include "mbedtls\dhm.h"
#include "mbedtls\ecdh.h"
#include "mbedtls\ecp.h"
#include "mbedtls\pk.h"
#include "mbedtls\ecp.h"
#include "mbedtls\x509_crt.h"
#include "mbedtls\bignum.h"
#include "mbedtls\ctr_drbg.h"
#include "mbedtls\ssl_ticket.h"
#include "mbedtls\ssl_cookie.h"
#include "mbedtls\base64.h"
#include "mbedtls\platform.h"
#include "mbedtls\memory_buffer_alloc.h"

#endif  // __ROM_EXPORT_MBEDTLS_H__