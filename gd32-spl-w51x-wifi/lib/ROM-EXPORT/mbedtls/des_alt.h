/**
 * \file des_alt.h
 *
 * \brief DES block cipher implemented by Freethink
 *
 */
#ifndef MBEDTLS_DES_ALT_H
#define MBEDTLS_DES_ALT_H

//#include "gd32f4xx_cryp.h"

#if defined(MBEDTLS_DES_ALT)

/**
* \brief			DES context structure
*/
typedef struct
{
	uint8_t mode;              /*1:ENCRPYT 0:DECRYPT*/
    uint8_t key[MBEDTLS_DES_KEY_SIZE]; 		   /*!<  DES keys	   */
}
mbedtls_des_context;

/**
* \brief		   Triple-DES context structure
*/
typedef struct
{
	uint8_t mode;              /*1:ENCRPYT 0:DECRYPT*/
    uint8_t key[MBEDTLS_DES_KEY_SIZE * 3];			/*!<  3DES keys		*/
}
mbedtls_des3_context;

void mbedtls_des_init( mbedtls_des_context *ctx );

void mbedtls_des_free( mbedtls_des_context *ctx );

void mbedtls_des3_init( mbedtls_des3_context *ctx );

void mbedtls_des3_free( mbedtls_des3_context *ctx );

int mbedtls_des_setkey_enc( mbedtls_des_context *ctx, const unsigned char key[MBEDTLS_DES_KEY_SIZE] );

int mbedtls_des_setkey_dec( mbedtls_des_context *ctx, const unsigned char key[MBEDTLS_DES_KEY_SIZE] );
void mbedtls_des_key_set_parity( unsigned char key[MBEDTLS_DES_KEY_SIZE] );

int mbedtls_des3_set2key_enc( mbedtls_des3_context *ctx,
				   const unsigned char key[MBEDTLS_DES_KEY_SIZE * 2] );

int mbedtls_des3_set2key_dec( mbedtls_des3_context *ctx,
				   const unsigned char key[MBEDTLS_DES_KEY_SIZE * 2] );

int mbedtls_des3_set3key_enc( mbedtls_des3_context *ctx,
				   const unsigned char key[MBEDTLS_DES_KEY_SIZE * 3] );
int mbedtls_des3_set3key_dec( mbedtls_des3_context *ctx,
				   const unsigned char key[MBEDTLS_DES_KEY_SIZE * 3] );

int mbedtls_des_crypt_ecb( mbedtls_des_context *ctx,
				 const unsigned char input[8],
				 unsigned char output[8] );

#if defined(MBEDTLS_CIPHER_MODE_CBC)
int mbedtls_des_crypt_cbc( mbedtls_des_context *ctx,
				 int mode,
				 size_t length,
				 unsigned char iv[8],
				 const unsigned char *input,
				 unsigned char *output );
#endif /* MBEDTLS_CIPHER_MODE_CBC */

int mbedtls_des3_crypt_ecb( mbedtls_des3_context *ctx,
				  const unsigned char input[8],
				  unsigned char output[8] );

#if defined(MBEDTLS_CIPHER_MODE_CBC)
int mbedtls_des3_crypt_cbc( mbedtls_des3_context *ctx,
				  int mode,
				  size_t length,
				  unsigned char iv[8],
				  const unsigned char *input,
				  unsigned char *output );
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#endif //MBEDTLS_DES_ALT

#endif //MBEDTLS_DES_ALT_H

