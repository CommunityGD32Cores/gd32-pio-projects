/*!
    \file    rom_pka.h
    \brief   Rom PKA interface for GD32W51x WiFi SDK

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

#ifndef __ROM_PKA_H__
#define __ROM_PKA_H__

#define POSITIVE 0
#define NEGATIVE 1

#define __LOCK		1
#define __UNLOCK	0

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

enum{
	MUTEX_TYPE_PKA = 0,
	MUTEX_TYPE_CRYP,
	MUTEX_TYPE_HASH,
	MUTEX_TYPE_MAX
};

/* RCU and Security config */
void rom_pka_config(void);

#if defined(PLATFORM_GDM32)
#if 0
/* Montgomery parameter compute */
/* res: (R exp 2) mod n */
int rom_pka_mpc(const uint32_t* n, uint32_t size, uint32_t* res);
/* modulus addition */
/* res: (A + B) mod n */
int rom_pka_modadd(IN const uint32_t* a, uint32_t a_size,
				IN const uint32_t* b, int b_size,
				IN const uint32_t* n, int n_size,
				OUT uint32_t* res);
/* modulus subtract */
/* res: (A - B) mod n */
int rom_pka_modsub(IN const uint32_t* a, uint32_t a_size,
				IN const uint32_t* b, int b_size,
				IN const uint32_t* n, int n_size,
				OUT uint32_t* res);
/* Montgomery multiple */
/* res: (A x B) mod n */
int rom_pka_modmul(IN const uint32_t* a, uint32_t a_size,
				IN const uint32_t* b, int b_size,
				IN const uint32_t* n, int n_size,
					OUT uint32_t* res);
#endif
/* modulus exponent */
/* res: (A exp e) mod n */
int rom_pka_modexp(IN const uint32_t* a,
				IN const uint32_t* e, int e_size,
				IN const uint32_t* n, int n_size,
				OUT uint32_t* res);
#if 0
/* modulus exponent fast */
/* res: (A exp e) mod n */
int rom_pka_modexp_fast(IN const uint32_t* a, uint32_t a_size,
				IN const uint32_t* e, int e_size,
				IN const uint32_t* n, int n_size,
				OUT uint32_t* res);
/* modulus inversion */
/* res: (A exp -1) mod n */
int rom_pka_modinv(IN const uint32_t* a, uint32_t a_size,
							IN  const uint32_t* n, uint32_t n_size,
							OUT uint32_t* res);
/* modulus reduction */
/* res: A mod n */
int rom_pka_modreduc(IN const uint32_t* a, uint32_t a_size,
								IN  const uint32_t* n, uint32_t n_size,
								OUT uint32_t* res);
#endif

/* Arithmetic Addition */
/* res: A + B */
int rom_pka_aadd(IN const uint32_t* a, uint32_t a_size,
						IN const uint32_t* b, uint32_t b_size,
						OUT uint32_t* res);
/* Arithmetic Subtraction */
/* res: A - B */
int rom_pka_asub(IN const uint32_t* a, uint32_t a_size,
						IN const uint32_t* b, uint32_t b_size,
						OUT uint32_t* res);
/* Arithmetic Multiplication */
/* res: A x B */
int rom_pka_amul(IN const uint32_t* a, uint32_t a_size,
						IN const uint32_t* b, uint32_t b_size,
						OUT uint32_t* res);
/* Arithmetic comparison */
/* res: A=B or A>B or A<B */
int rom_pka_acomp(IN const uint32_t* a, uint32_t a_size,
						IN const uint32_t* b, uint32_t b_size,
						OUT uint32_t* res);

/* ECDSA sign */
int rom_pka_ecdsa_sign(IN uint8_t* d,
  							 IN uint8_t* k,
  							 IN uint8_t* hash,
  							 IN void* group,
  							 OUT uint8_t* r,
  							 OUT uint8_t* s);
/* ECDSA verify */
int rom_pka_ecdsa_verify(IN uint8_t *r,
									 IN uint8_t *s,
									 IN uint8_t *hash,
									 IN uint8_t *pk_x,
									 IN uint8_t *pk_y,
									 IN void *group);
/* return 0: (x, y) on curve */
uint32_t rom_pka_ecp_check(IN void *group,
								 IN uint8_t *x,
								 IN uint8_t *y);
/* x_res: k x P coordinate x */
/* y_res: k x P coordinate y */
int rom_pka_ecp_mul(IN void *group,
						 IN uint8_t *px,
						 IN uint8_t *py,
						 IN uint8_t *k,
						 IN uint32_t k_size,
						 OUT uint8_t *rx,
						 OUT uint8_t *ry);
#if 0
/* x_res: k x P coordinate x */
/* y_res: k x P coordinate y */
int rom_pka_ecp_mul_fast(IN pka_ecgrp_t* group,
									 IN uint32_t* x,
									 IN uint32_t* y,
									 IN uint32_t* k,
									 IN uint32_t k_size,
									 OUT uint32_t* x_res,
									 OUT uint32_t* y_res);
/* res: (A exp d) mod (p x q) */
int rom_pka_rsa_crt(IN uint32_t* dp,
						IN uint32_t* dq,
						IN uint32_t* qinv,
						IN uint32_t* p,
						IN uint32_t* q,
						IN uint32_t M_size,
						IN uint32_t* A,
						IN uint32_t A_size,
						OUT uint8_t* res);
#endif

#endif  /* PLATFORM_GDM32 */

#ifdef ROM_SELF_TEST
void pka_self_test(void);
#endif

#endif  /* __ROM_PKA_H__ */

